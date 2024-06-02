#include "main.h"
#include "sysInit.h"
#include "utils.h"

#include "uart_dma.hpp"
#include "timer.hpp"

//#include "motor.hpp"
//#include "encoder.hpp"
#include "calibrator.hpp"
#include "controller.hpp"

#include <cstdint>
#include <cstring>

Motor motor;
Encoder encoder;

Calibrator calibrator(&motor, &encoder);
Controller controller(&motor, &encoder);

Timer timer50us(Tick20khz);
Timer timer100ms(Tick10hz);
Timer timer500ms(Tick2hz);

UartDMA uart;
#define BUFFER_SIZE 128
uint8_t uartSendBuf[BUFFER_SIZE];
uint8_t uartCount = 0;

int count_100ms = 0;
int count_500ms = 0;

void loop100ms(void *context);
void loop50us(void *context);
void loop500ms(void *context);


int main() {
    System_Init();

    encoder.init();
    // get start position.
    motor.init();

    calibrator.Calibration_Init();
    controller.Init();

    uart.init();
    HAL_Delay(100);

    //timer100ms.init();
    //timer500ms.init();
    timer50us.init();

    HAL_Delay(10);

    //timer100ms.setCallback(loop100ms, nullptr);
    timer50us.setCallback(loop50us, nullptr);
    //timer500ms.setCallback(loop500ms, nullptr);

    //timer100ms.start();
    //timer500ms.start();
    timer50us.start();

    while (1) {
    }

    return 0;
}

// main loop
void loop50us(void *context) {
    if (calibrator.isTriggered()) {
        calibrator.Calibration_Interrupt_Callback();
    }
    else {
        controller.Callback();
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