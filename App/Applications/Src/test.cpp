#include "main.h"
#include "sysInit.h"
#include "utils.h"

#include "mt6816.hpp"
#include "tb67h450.hpp"
#include "uart_dma.hpp"
#include "timer.hpp"

#include <cstdint>
#include <cstring>

#include "sin_map.h"


//uint16_t MT6816Read();
void stepMotor(int step);
void stepMotor8(int step);
void stepMotorStop();

typedef struct
{
    uint16_t sinMapPtr;
    int16_t sinMapData;
    uint16_t dacValue12Bits;
} FastSinToDac_t;

FastSinToDac_t phaseB{};
FastSinToDac_t phaseA{};
void SetFocCurrentVector(uint32_t _directionInCount, int32_t _current_mA);

uint16_t goPosition = 0;
MT6816 mt6816;
spiRawData_t angleData;
TB67H450 tb67h450;

Timer timer50us(Tick20khz);
Timer timer100ms(Tick10hz);
Timer timer500ms(Tick2hz);

UartDMA uart;
#define BUFFER_SIZE 128
uint8_t uartSendBuf[BUFFER_SIZE];
uint8_t uartTestBuf[BUFFER_SIZE];
uint16_t prevPosition = 0;
uint8_t uartCount = 0;


#define STEP_PER_CIRCLE 4
int rotations = 100;  // 50 teeth
int i = 0;
bool direction = true;
int subDivision = 50;
int subDivCount = 0;

int count_100ms = 0;
int count_500ms = 0;

// for speed test.
#define ENCODER_BUF 256 * 2
uint16_t encoder_buf[ENCODER_BUF];
uint16_t buf_conut = 0;
bool stop_moving = false;
bool first_time = true;
uint32_t position = 0;
uint32_t step = 0;
uint16_t division = 0;

void loop100ms(void *context);
void loop50us(void *context);
void loop500ms(void *context);

int main() {
    System_Init();

    mt6816.init();
    // get start position.
    mt6816.readAngle(angleData);
    prevPosition = angleData.data;
    uart.init();
    HAL_Delay(100);
    // motor driver init
    tb67h450.init();
    
    HAL_Delay(100);
    tb67h450.setVrefA(350);
    tb67h450.setVrefB(350);

    //printf("initialization done.\n");

    timer100ms.init();
    timer500ms.init();
    timer50us.init();

    HAL_Delay(10);
    //printf("start timer.\n");

    timer100ms.setCallback(loop100ms, nullptr);
    timer50us.setCallback(loop50us, nullptr);
    timer500ms.setCallback(loop500ms, nullptr);

    //timer100ms.start();
    //timer500ms.start();
    SetFocCurrentVector(0, 2000);
    HAL_Delay(500);
    timer50us.start();
    //printf("start timer ok.\n");

    while (1) {

        if (stop_moving) {
            //SetFocCurrentVector(0, 1000);
            //printf("stop movind.\n");
            for (uint16_t i = 0; i < ENCODER_BUF; i++) {
                //if (uart.m_txComplete) {
                // uint16_t len = snprintf((char*)uartSendBuf, BUFFER_SIZE, "%u\n", angleData.data);
                // uart.transmit((uint8_t*)uartSendBuf, len);
                printf("%u, %u\n", i, encoder_buf[i]);
            }
            //SetFocCurrentVector(0, 0);
            break;
        }
    }

    return 0;
}

// main loop
void loop50us(void *context) {
    if (!stop_moving) {
        mt6816.readAngle(angleData);
        encoder_buf[buf_conut] = angleData.data;
        buf_conut++;
        if (buf_conut % 128 == 0) {
            position += 256;
            SetFocCurrentVector(position, 2000);
        }
        //position += 256;
        //SetFocCurrentVector(position, 2000);
        if (buf_conut >= ENCODER_BUF) {
            stop_moving = true;
            SetFocCurrentVector(256, 0);
        }
    }
    
    // 逐渐加速
    // division++;
    // if (division <= 2) {
    //     return;
    // }
    // else {
    //     division = 0;
    // }

    // if (step >= 128) {
    //     step = 128;
    // }
    // if (position < 52100) {
    //     mt6816.readAngle(angleData);
    //     position += step;
    //     SetFocCurrentVector(position, 2000);
    //     step++;
    // }
    // else {
    //     mt6816.readAngle(angleData);
    //     position += step;
    //     SetFocCurrentVector(position, 2000);
    //     step++;
    //     position = position % 51200;
    // }
}

void loop100ms(void *context) {
    if (count_100ms > 1000) {
        count_100ms = 0;
    }
    count_100ms++;
}

void loop500ms(void *context) {
    if (count_500ms > 1000) {
        count_500ms = 0;
    }
    count_500ms++;
}

void stepMotor(int step)
{
    switch(step % STEP_PER_CIRCLE)
    {
        case 0:
            tb67h450.setOutptA(true, false);
            tb67h450.setOutptB(false, false);
            break;
        case 1:
            tb67h450.setOutptA(false, false);
            tb67h450.setOutptB(true, false);
            break;
        case 2:
            tb67h450.setOutptA(false, true);
            tb67h450.setOutptB(false, false);
            break;
        case 3:
            tb67h450.setOutptA(false, false);
            tb67h450.setOutptB(false, true);
            break;
    }
}

void stepMotor8(int step)
{
    switch(step % 8)
    {
        case 0:
            tb67h450.setOutptA(true, false);
            tb67h450.setOutptB(false, false);
            break;
        case 1:
            tb67h450.setOutptA(true, false);
            tb67h450.setOutptB(true, false);
            break;
        case 2:
            tb67h450.setOutptA(false, false);
            tb67h450.setOutptB(true, false);
            break;
        case 3:
            tb67h450.setOutptA(false, true);
            tb67h450.setOutptB(true, false);
            break;
        case 4:
            tb67h450.setOutptA(false, true);
            tb67h450.setOutptB(false, false);
            break;
        case 5:
            tb67h450.setOutptA(false, true);
            tb67h450.setOutptB(false, true);
            break;
        case 6:
            tb67h450.setOutptA(false, false);
            tb67h450.setOutptB(false, true);
            break;
        case 7:
            tb67h450.setOutptA(true, false);
            tb67h450.setOutptB(false, true);
            break;
    }
}

void stepMotorStop() {
    tb67h450.setOutptA(false, false);
    tb67h450.setOutptB(false, false);
}

void SetFocCurrentVector(uint32_t _directionInCount, int32_t _current_mA)
{
    phaseB.sinMapPtr = (_directionInCount) & (0x000003FF);// 限制在1023内
    phaseA.sinMapPtr = (phaseB.sinMapPtr + (256)) & (0x000003FF);// B相延后四分之一周期

    phaseA.sinMapData = sin_pi_m2[phaseA.sinMapPtr];// 取对应sin值
    phaseB.sinMapData = sin_pi_m2[phaseB.sinMapPtr];

    uint32_t dac_reg = fabs(_current_mA);// 绝对值
    dac_reg = (uint32_t)(dac_reg * 5083) >> 12; // 3300转4095
    dac_reg = dac_reg & (0x00000FFF);
    phaseA.dacValue12Bits =
        (uint32_t)(dac_reg * fabs(phaseA.sinMapData)) >> sin_pi_m2_dpiybit;// 由实际细分数得到实际电流（对标4095）
    phaseB.dacValue12Bits =
        (uint32_t)(dac_reg * fabs(phaseB.sinMapData)) >> sin_pi_m2_dpiybit;

    // SetTwoCoilsCurrent(phaseA.dacValue12Bits, phaseB.dacValue12Bits);
    //DacOutputVoltage(phaseA.dacValue12Bits, phaseB.dacValue12Bits);
    tb67h450.setVrefA(phaseA.dacValue12Bits >> 2);
    tb67h450.setVrefB(phaseB.dacValue12Bits >> 2);
    if (phaseA.sinMapData > 0)
        tb67h450.setOutptA(true, false);
    else if (phaseA.sinMapData < 0)
        tb67h450.setOutptA(false, true);
    else
        tb67h450.setOutptA(true, true);

    if (phaseB.sinMapData > 0)
        tb67h450.setOutptB(true, false);
    else if (phaseB.sinMapData < 0)
        tb67h450.setOutptB(false, true);
    else
        tb67h450.setOutptB(true, true);
}