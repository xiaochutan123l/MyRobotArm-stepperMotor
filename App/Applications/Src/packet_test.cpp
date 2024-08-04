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
#include "button.hpp"

Motor motor;
Encoder encoder;

Controller controller(&motor, &encoder);
Calibrator calibrator(&motor, &encoder, &controller);

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
bool getCommandReceived = false;
uint32_t uartSendBufOffset = 0;

Button button1(BUTTON_1);
Button button2(BUTTON_2);

int count_100ms = 0;
int count_500ms = 0;

void loop100ms();
void loop50us();
void loop500ms();
void handle_packet();

uint32_t GetMicros(void); 
uint32_t elapsed_time = 0;

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

    button1.init();
    button2.init();

    motor_config.updateConfig();
    // set config with default value.
    // if (motor_config.getConfig().configStatus != CONFIG_OK) {
    //     // TODO: set default value.
    // }
    // if (!motor_config.getConfig().calibrated) {
    //     calibrator.trigger();
    // }
    if (!button1.isPressed() && !button2.isPressed()) {
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
    //uint32_t start_t = GetMicros();
    if (calibrator.isTriggered()) {
        calibrator.Calibration_Interrupt_Callback();
    }
    else {
        handle_packet();
        // main control loop
        controller.Callback();

        if (uart.m_txComplete) {
            if (getCommandReceived) {
                uart.transmit((uint8_t*)uartSendBuf, uartSendBufOffset);
                uartSendBufOffset = 0;
                getCommandReceived = false;
            }
            else {
                uint16_t len = snprintf((char*)uartSendBuf, BUFFER_SIZE, "test: %ld,%ld\n" , controller.m_est_location, controller.m_real_lap_location);
                //uint16_t len = snprintf((char*)uartSendBuf, BUFFER_SIZE, "test: %ld,%ld\n" , controller.m_est_speed, controller.m_goal_speed);
                uart.transmit((uint8_t*)uartSendBuf, len);
            }
        }
        //elapsed_time = GetMicros() - start_t;
    } 
    
}

void handle_packet() {
    if (packet_handler.is_new_packet_received()) {
        switch(packet_handler.getPacket().cmdNum) {
            case CMD_UNKNOWN: {
                break;
            }
            case SetPidKp: {
                controller.m_pid.SetKP(GET_DATA_INT(&packet_handler.getPacket()));
                break;
            }
            case SetPidKi: {
                controller.m_pid.SetKI(GET_DATA_INT(&packet_handler.getPacket()));
                break;
            }
            case SetPidKd: {
                controller.m_pid.SetKD(GET_DATA_INT(&packet_handler.getPacket()));
                break;
            }
            case SetDceKp: {
                controller.m_dce.SetKP(GET_DATA_INT(&packet_handler.getPacket()));
                break;
            }
            case SetDceKv: {
                controller.m_dce.SetKV(GET_DATA_INT(&packet_handler.getPacket()));
                break;
            }
            case SetDceKi: {
                controller.m_dce.SetKI(GET_DATA_INT(&packet_handler.getPacket()));
                break;
            }
            case SetDceKd: {
                controller.m_dce.SetKD(GET_DATA_INT(&packet_handler.getPacket()));
                break;
            }
            case SetPosition: {
                controller.Write_Goal_Location(GET_DATA_INT(&(packet_handler.getPacket())));
                break;
            }
            case SetVelocity: {
                controller.Write_Goal_Speed(GET_DATA_INT(&(packet_handler.getPacket())));
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
            // Currently for testing purpose.
            // 之后估计要考虑有一个方案一个指令同时设置location和max speed
            case SetVelocityLimit: {
                // 设置最大速度，让电机指定速度运动到接下来给定的位置。
                controller.Set_Location_Mode_Max_Speed(GET_DATA_INT(&(packet_handler.getPacket())));
                controller.SetMotorMode(Motor_Mode_Digital_Location);
                break;
            }
            case GetPosition: {
                getCommandReceived = true;
                packet_handler.getPacket().data = DATA_TO_UINT(controller.m_est_location);
                memcpy(uartSendBuf + uartSendBufOffset, &packet_handler.getPacket(), PACKET_LEN);
                uartSendBufOffset += PACKET_LEN;
                break;
            }
            case GetVelocity: {
                getCommandReceived = true;
                packet_handler.getPacket().data = DATA_TO_UINT(controller.m_est_speed);
                memcpy(uartSendBuf + uartSendBufOffset, &packet_handler.getPacket(), PACKET_LEN);
                uartSendBufOffset += PACKET_LEN;
                break;
            }
            case Stop: {
                controller.SetMotorMode(Control_Mode_Stop);
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

// 假设系统时钟频率为 72 MHz（适用于STM32F1系列）
#define SYS_CLOCK_FREQ 72000000 // 72 MHz

uint32_t GetMicros(void) {
    uint32_t micros;
    uint32_t sysTickVal;
    uint32_t loadVal;

    // 获取SysTick当前值和重装载值
    sysTickVal = SysTick->VAL;
    loadVal = SysTick->LOAD;

    // 计算当前微秒数
    micros = (HAL_GetTick() * 1000) + (loadVal - sysTickVal) / (SYS_CLOCK_FREQ / 1000000);

    return micros;
}