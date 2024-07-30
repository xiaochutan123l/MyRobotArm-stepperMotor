#include "main.h"
#include "sysInit.h"
#include "utils.h"
#include "uart_dma.hpp"
#include "timer.hpp"
#include "packet_handler.hpp"
#include "calibrator.hpp"
#include "controller.hpp"
#include "motor_config.hpp"
#include <cstdint>
#include <cstring>
#include "protocol.h"

Motor motor;
Encoder encoder;

Calibrator calibrator(&motor, &encoder);
Controller controller(&motor, &encoder);

MotorConfig motor_config;

Timer timer50us(Tick20khz);
Timer timer100ms(Tick10hz);
Timer timer500ms(Tick2hz);

// UartDMA uart;
#define BUFFER_SIZE 128
uint8_t uartSendBuf[BUFFER_SIZE];
uint8_t uartCount = 0;

UartDMA uart;
PacketHandler packet_handler;

int count_100ms = 0;
int count_500ms = 0;

void loop100ms();
void loop50us();
void loop500ms();


int main() {
    System_Init();

    encoder.init();
    // get start position.
    motor.init();

    calibrator.Calibration_Init();
    controller.Init();

    uart.init();
    uart.setIDLECallback(PacketHandler::packet_process);
    HAL_Delay(100);

    motor_config.updateConfig();
    // set config with default value.
    if (motor_config.getConfig().configStatus != CONFIG_OK) {
        // TODO: set default value.
    }
    if (!motor_config.getConfig().calibrated) {
        calibrator.trigger();
    }

    //timer100ms.init();
    //timer500ms.init();
    timer50us.init();

    HAL_Delay(10);

    //timer100ms.setCallback(loop100ms);
    timer50us.setCallback(loop50us);
    //timer500ms.setCallback(loop500ms);

    //timer100ms.start();
    //timer500ms.start();
    timer50us.start();

    while (1) {
        calibrator.Calibration_Loop_Callback();
        if (motor_config.getConfig().configStatus == CONFIG_COMMIT) {
            motor_config.getConfig().configStatus = CONFIG_OK;
            motor_config.write_data_to_flash();
        } 
        else if (motor_config.getConfig().configStatus == CONFIG_RESTORE) {
            motor_config.write_data_to_flash();
            HAL_NVIC_SystemReset();
        }
    }

    return 0;
}

void loop50us() {
    if (calibrator.isTriggered()) {
        calibrator.Calibration_Interrupt_Callback();
    }
    else {
        controller.Callback();
        if (uart.m_txComplete) {
            uint16_t len = snprintf((char*)uartSendBuf, BUFFER_SIZE, "test: %ld,%ld\n" , controller.m_est_location, controller.m_real_lap_location);
            uart.transmit((uint8_t*)uartSendBuf, len);
        }
    } 
}

void handle_packet() {
    if (packet_handler.is_new_packet_received()) {
        switch(packet_handler.getPacket().cmdNum) {
            case CMD_UNKNOWN: {
                break;
            }
            case SetPidKp: {
                controller.m_pid.SetKP(GET_RAW_DATA_INT(&packet_handler.getPacket()));
                break;
            }
            case SetPidKi: {
                controller.m_pid.SetKI(GET_RAW_DATA_INT(&packet_handler.getPacket()));
                break;
            }
            case SetPidKd: {
                controller.m_pid.SetKD(GET_RAW_DATA_INT(&packet_handler.getPacket()));
                break;
            }
            case SetDceKp: {
                controller.m_dce.SetKP(GET_RAW_DATA_INT(&packet_handler.getPacket()));
                break;
            }
            case SetDceKv: {
                controller.m_dce.SetKV(GET_RAW_DATA_INT(&packet_handler.getPacket()));
                break;
            }
            case SetDceKi: {
                controller.m_dce.SetKI(GET_RAW_DATA_INT(&packet_handler.getPacket()));
                break;
            }
            case SetDceKd: {
                controller.m_dce.SetKD(GET_RAW_DATA_INT(&packet_handler.getPacket()));
                break;
            }
            case SetPosition: {
                controller.Write_Goal_Location(GET_RAW_DATA_INT(&packet_handler.getPacket()));
                break;
            }
            case SetVelocity: {
                controller.Write_Goal_Speed(GET_RAW_DATA_INT(&packet_handler.getPacket()));
                break;
            }
            case SetSpeedMode: {
                controller.SetMotorMode(Motor_Mode_Digital_Speed);
                break;
            }
            case SetLocationMode: {
                controller.SetMotorMode(Motor_Mode_Digital_Location);
                break;
            }
            case GetPosition: {
                if (uart.m_txComplete) {
                    packet_handler.getPacket().data = DATA_TO_UINT(controller.m_est_location);
                    memcpy(uartSendBuf, &packet_handler, sizeof(packet_handler));
                    uart.transmit((uint8_t*)uartSendBuf, sizeof(packet_handler));
                }
                break;
            }
            case GetVelocity: {
                if (uart.m_txComplete) {
                    packet_handler.getPacket().data = DATA_TO_UINT(controller.m_est_speed);
                    memcpy(uartSendBuf, &packet_handler, sizeof(packet_handler));
                    uart.transmit((uint8_t*)uartSendBuf, sizeof(packet_handler));
                }
                break;
            }
            case DoCalibrate: {
                calibrator.trigger();
                break;
            }
            case Reboot: {
                HAL_NVIC_SystemReset();
                break;
            }
            default: break;
        }
        packet_handler.set_packet_processed();
    }
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