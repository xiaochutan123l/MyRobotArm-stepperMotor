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
uint8_t uartCount = 0;

int count_100ms = 0;
int count_500ms = 0;

void loop100ms();
void loop50us();
void loop500ms();


// volecity pid
int32_t speed = 0;
int32_t kp = 0;
int32_t ki = 0;
int32_t kd = 0;
int32_t currentPosition = 0;
int32_t prevPosition = 0;
int32_t velocity = 0;
int32_t positionDiff = 0;
int32_t speedDiff = 0;
int32_t prevSpeed = 0;
int32_t speedError = 0;
int32_t prevSpeedError = 0;
int32_t speedDerivative = 0;
int32_t pidOutput = 0;
int32_t integral = 0;
#define MAX_CURRENT 1000 // ma
#define MAX_SPEED 256 // max 256 subdivision
void calcVelocityPid(int32_t _speed);


int32_t targetSpeed = 5; // max 81.92
uint32_t count = 0;

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

    timer100ms.setCallback(loop100ms);
    timer50us.setCallback(loop50us);
    timer500ms.setCallback(loop500ms);

    //timer100ms.start();
    //timer500ms.start();
    timer50us.start();
    //printf("start timer ok.\n");

    while (1) {
    }

    return 0;
}

// main loop
void loop50us() {
    // if (goPosition <= 51200)
    // //if (goPosition <= 9000)
    // {
    //     // 测试：以2000mA电流跑一圈
    //     mt6816.readAngle(angleData);
    //     uartCount++;
    //     if (uartCount == 200 && uart.m_txComplete) {
    //         uint16_t len = snprintf((char*)uartSendBuf, BUFFER_SIZE, "%u,%u\n", angleData.data, goPosition);
    //         uart.transmit((uint8_t*)uartSendBuf, len);
    //         uartCount = 0;
    //     }
        
    //     SetFocCurrentVector(goPosition, 1000);
    //     goPosition += 2;
    // }
    // else {
    //     goPosition = 0;
    // }
    mt6816.readAngle(angleData);
    positionDiff = angleData.data - prevPosition;
    prevPosition = angleData.data;
    if (uart.m_txComplete) {
            uint16_t len = snprintf((char*)uartSendBuf, BUFFER_SIZE, "%ld, %u\n", positionDiff, angleData.data);
            uart.transmit((uint8_t*)uartSendBuf, len);
        }
    //int32_t count = 256 * (targetSpeed / 81.92) + 256;
    count = count + 1;
    //count += 256;
    if (count >= 51200) {
        count = count % 51200;
    }
    //SetFocCurrentVector(256 * (targetSpeed / 81.92), 1000);
    SetFocCurrentVector(count, 500);

}

void calcVelocityPid(int32_t _speed) {
    // speed: 转动的角度，用mt6816的编码值代替
    positionDiff = currentPosition - prevPosition;
    prevSpeed = positionDiff;

    speedError = _speed - prevSpeed;

    speedDerivative = (speedError - prevSpeedError) / 1;
    integral += speedError;
    pidOutput = (kp * speedError) + (kd * speedDerivative);
    prevSpeedError = speedError;
}

void loop100ms() {
    if (count_100ms > 1000) {
        count_100ms = 0;
    }
    count_100ms++;
}

void loop500ms() {
    if (count_500ms > 1000) {
        count_500ms = 0;
    }
    count_500ms++;
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
