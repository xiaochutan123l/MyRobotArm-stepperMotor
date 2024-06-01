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

// uint32_t count = 0;
uint32_t count = 0;
bool printData = true;
bool checkError = true;
uint16_t cali_data_num = 201;
uint16_t *cali_data = (uint16_t*)CALI_DATA_ADDR;

int main() {
    System_Init();
    calibrator.Calibration_Init();
    calibrator.trigger();
    uart.init();
    HAL_Delay(100);
    
    HAL_Delay(100);

    timer100ms.init();
    timer500ms.init();
    timer50us.init();

    HAL_Delay(10);
    printf("start timer.\n");

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

void loop50us(void *context) {
    if (calibrator.isTriggered()) {
        calibrator.Calibration_Interrupt_Callback();
        // if (uart.m_txComplete) {
        //     //uint16_t len = snprintf((char*)uartSendBuf, BUFFER_SIZE, "%ld, %u\n", count, cali_data[count]);
        //     uint16_t len = snprintf((char*)uartSendBuf, BUFFER_SIZE, "%ld\n", count);
            
        //     uart.transmit((uint8_t*)uartSendBuf, len);
        //     count++;
        // }
    }
    else if (printData){
        if (checkError) {
            if (uart.m_txComplete) {
            uint16_t len = snprintf((char*)uartSendBuf, BUFFER_SIZE, "error: %ld, %ld\n", calibrator.m_encode_cali.error_code, calibrator.m_encode_cali.error_data);
            uart.transmit((uint8_t*)uartSendBuf, len);
            checkError = false;
        }
        }
        // print all cali data.
        if (uart.m_txComplete) {
            //uint16_t len = snprintf((char*)uartSendBuf, BUFFER_SIZE, "%ld, %u, %u\n", count, calibrator.m_encode_cali.coder_data_r[count], calibrator.m_encode_cali.coder_data_f[count]);
            uint16_t len = snprintf((char*)uartSendBuf, BUFFER_SIZE, "%ld, %u\n", count, cali_data[count]);
            
            uart.transmit((uint8_t*)uartSendBuf, len);
            count++;
        }
        if (count >= cali_data_num) {
            printData = false;
        }
        
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