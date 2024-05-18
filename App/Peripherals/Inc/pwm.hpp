#ifndef PWM_HPP
#define PWM_HPP

#include "tim.h"
#include "peripheral_config.h"

class PWM {
public:
    PWM(uint32_t channel);
    void init();
    // Period 1000.
    inline void setVoltage(uint16_t value) {__HAL_TIM_SET_COMPARE(m_htim, m_channel, value);};

    static bool m_isInitialized;

    TIM_HandleTypeDef *m_htim;
    uint32_t m_channel;

};

#endif // PWM_HPP