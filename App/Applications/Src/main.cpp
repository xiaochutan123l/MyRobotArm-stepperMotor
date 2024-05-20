#include "main.h"
#include "sysInit.h"
#include "utils.h"

#include "mt6816.hpp"
#include "tb67h450.hpp"
#include "uart_dma.hpp"
#include "timer.hpp"

#include <cstdint>
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

int goPosition = 0;
MT6816 mt6816;
spiRawData_t angleData;
TB67H450 tb67h450;

Timer timer50us(Tick20khz);
Timer timer100ms(Tick10hz);
Timer timer500ms(Tick2hz);

UartDMA uart;

#define STEP_PER_CIRCLE 4
int rotations = 100;  // 50 teeth
int i = 0;
bool direction = true;
int subDivision = 50;
int subDivCount = 0;

int count_100ms = 0;
int count_500ms = 0;

void loop100ms(void *context);
void loop50us(void *context);
void loop500ms(void *context);

int main() {
    System_Init();

    mt6816.init();
    
    uart.init();
    HAL_Delay(100);
    // motor driver init
    tb67h450.init();
    
    HAL_Delay(100);
    tb67h450.setVrefA(350);
    tb67h450.setVrefB(350);

    printf("initialization done.\n");

    timer100ms.init();
    timer500ms.init();
    timer50us.init();

    HAL_Delay(10);
    printf("start timer.\n");

    timer100ms.setCallback(loop100ms, nullptr);
    timer50us.setCallback(loop50us, nullptr);
    timer500ms.setCallback(loop500ms, nullptr);

    timer100ms.start();
    timer500ms.start();
    timer50us.start();
    printf("start timer ok.\n");

    while (1) {
    }

    return 0;
}

void loop50us(void *context) {
    if (goPosition <= 51200)
    {
        // 测试：以2000mA电流跑一圈
        mt6816.readAngle(angleData);
        SetFocCurrentVector(goPosition, 1000);
        goPosition += 2;
    }
    else {
        goPosition = 0;
    }
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
