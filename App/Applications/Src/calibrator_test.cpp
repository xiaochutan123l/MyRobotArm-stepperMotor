#include "main.h"
#include "sysInit.h"
#include "utils.h"

#include "calibrator.hpp"
#include "uart_dma.hpp"
#include "timer.hpp"

#include <cstdint>
#include <cstring>

Timer timer50us(Tick20khz);
Timer timer100ms(Tick10hz);
Timer timer500ms(Tick2hz);

Calibrator calibrator;

UartDMA uart;
#define BUFFER_SIZE 128
uint8_t uartSendBuf[BUFFER_SIZE];
uint8_t uartCount = 0;

int count_100ms = 0;
int count_500ms = 0;

void loop100ms(void *context);
void loop50us(void *context);
void loop500ms(void *context);

uint32_t count = 0;

int main() {
    System_Init();
    calibrator.Calibration_Init();
    uart.init();
    HAL_Delay(100);
    
    HAL_Delay(100);

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
    timer50us.start();
    //printf("start timer ok.\n");

    while (1) {
        calibrator.Calibration_Loop_Callback();
    }

    return 0;
}

// main loop
// void loop50us(void *context) {
//     mt6816.readAngle(angleData);
//     positionDiff = angleData.data - prevPosition;
//     prevPosition = angleData.data;
//     if (uart.m_txComplete) {
//             uint16_t len = snprintf((char*)uartSendBuf, BUFFER_SIZE, "%ld, %u\n", positionDiff, angleData.data);
//             uart.transmit((uint8_t*)uartSendBuf, len);
//         }
//     //int32_t count = 256 * (targetSpeed / 81.92) + 256;
//     count = count + 1;
//     //count += 256;
//     if (count >= 51200) {
//         count = count % 51200;
//     }
//     //SetFocCurrentVector(256 * (targetSpeed / 81.92), 1000);
//     SetFocCurrentVector(count, 500);

// }

void loop50us(void *context) {
    calibrator.Calibration_Interrupt_Callback();
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