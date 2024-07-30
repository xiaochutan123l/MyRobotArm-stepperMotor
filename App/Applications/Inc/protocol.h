#ifndef PROTOCOL_H
#define PROTOCOL_H

/*
基于uart和can的通用电机控制通信协议。一共8byte
0         3           7
+---------+-----------+
|   CMD   |    Data   | 
+---------+-----------+

reference: https://github.com/peng-zhihui/Dummy-Robot/blob/main/2.Firmware/Ctrl-Step-Driver-STM32F1-fw/UserApp/protocols/interface_can.cpp

- CMD 4 Byte: Command field (or 4bit is enough？)
  - classification:

    1. 无需存储的实时控制命令
        0x01 - Enable Motor
        启动或停止电机
        0x02 - Do Calibration
        触发编码器校准
        0x03 - Set Current SetPoint
        设置电流设定点
        0x04 - Set Velocity SetPoint
        设置速度设定点
        0x05 - Set Position SetPoint
        设置位置设定点，并根据需要返回ACK
        0x06 - Set Position with Time
        设置带有时间限制的位置设定点，并根据需要返回ACK
        0x07 - Set Position with Velocity-Limit
        设置带有速度限制的位置设定点，并始终返回ACK

    2. 存储类配置命令
        0x11 - Set Node-ID and Store to EEPROM
        设置节点ID并存储到EEPROM
        0x12 - Set Current-Limit and Store to EEPROM
        设置电流限制并存储到EEPROM
        0x13 - Set Velocity-Limit and Store to EEPROM
        设置速度限制并存储到EEPROM
        0x14 - Set Acceleration and Store to EEPROM
        设置加速度并存储到EEPROM
        0x15 - Apply Home-Position and Store to EEPROM
        应用原点位置并存储到EEPROM
        0x16 - Set Auto-Enable and Store to EEPROM
        设置自动启用并存储到EEPROM
        0x17 - Set DCE Kp
        设置DCE的比例增益并存储到EEPROM
        0x18 - Set DCE Kv
        设置DCE的速度增益并存储到EEPROM
        0x19 - Set DCE Ki
        设置DCE的积分增益并存储到EEPROM
        0x1A - Set DCE Kd
        设置DCE的微分增益并存储到EEPROM
        0x1B - Set Enable Stall-Protect
        启用防卡保护并存储到EEPROM

    3. 查询命令
        0x21 - Get Current
        获取当前电流
        0x22 - Get Velocity
        获取当前速度
        0x23 - Get Position
        获取当前位置
        0x24 - Get Offset
        获取编码器偏移量

    4. 系统维护和重置命令
        0x7E - Erase Configs
        擦除配置
        0x7F - Reboot
        重启系统

- Data 4 Byte: max 32bit data, type independent. (int or uint or float)
*/


#ifdef __cplusplus
extern "C" {
#endif


enum DataType {
    UINT,
    INT,
    FLOAT,
    BOOL,
    UNKNOWN
};

enum CMDType {
    CMD_UNKNOWN=0,
    SetPidKp=1,
    SetPidKi,
    SetPidKd,
    SetDceKp,
    SetDceKv,
    SetDceKi,
    SetDceKd,
    SetPosition,
    SetVelocity,
    SetSpeedMode,
    SetLocationMode,
    GetPosition,
    GetVelocity,
    DoCalibrate,
    Reboot,
};

#ifdef __cplusplus
}
#endif

#endif //PROTOCOL_H
