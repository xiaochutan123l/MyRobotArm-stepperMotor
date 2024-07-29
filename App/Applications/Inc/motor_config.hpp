#ifndef MOTOR_CONFIG_HPP
#define MOTOR_CONFIG_HPP

#ifdef __cplusplus
extern "C"{
#endif

#include <stdint.h>
#include "soft_eeprom.hpp"

typedef enum configStatus_t : uint32_t
{
    CONFIG_RESTORE = 0,
    CONFIG_OK,
    CONFIG_COMMIT
} configStatus_t;


struct Motor_Config_t
{
    configStatus_t configStatus;
    uint32_t canNodeId;
    int32_t encoderHomeOffset;
    uint32_t defaultMode;
    int32_t currentLimit;
    int32_t velocityLimit;
    int32_t velocityAcc;
    int32_t calibrationCurrent;
    int32_t pid_kp;
    int32_t pid_ki;
    int32_t pid_kd;
    int32_t dce_kp;
    int32_t dce_kv;
    int32_t dce_ki;
    int32_t dce_kd;
    bool calibrated;
    bool enableMotorOnBoot;
    bool enableStallProtect;
};

class MotorConfig {
public:
    MotorConfig() : m_eeprom(USER_DATA) {}
    inline void write_data_to_flash() {
        m_eeprom.Write_Data32(&m_motor_config, sizeof(Motor_Config_t));
    }
    inline Motor_Config_t& getConfig() {
        return m_motor_config;
    }
    inline void updateConfig() {
        m_eeprom.Read_Data32(&m_motor_config, sizeof(Motor_Config_t));
    }
private:
    Motor_Config_t m_motor_config;
    Soft_EEPROM m_eeprom;
};

#ifdef __cplusplus
}
#endif

#endif // MOTOR_CONFIG_HPP