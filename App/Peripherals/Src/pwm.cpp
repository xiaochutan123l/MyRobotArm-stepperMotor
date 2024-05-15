#include "pwm.hpp"

bool PWM::m_isInitialized = false;

PWM::PWM(uint32_t channel) : m_channel(channel) {

}

void PWM::init() {
    if (!m_isInitialized) {
        MX_TIM3_Init();
        m_isInitialized = true;
    }
    // switch case htim numer.
    m_htim = &htim3;
    HAL_TIM_PWM_Start(m_htim, m_channel);
}