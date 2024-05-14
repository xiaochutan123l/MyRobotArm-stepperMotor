#include "adc.hpp"

bool ADC::m_isInitialized = false;

ADC::ADC(uint8_t channel) : m_channel(channel) {
    if (!m_isInitialized) {
        MX_ADC1_Init();
        m_isInitialized = true;
    }
    //HAL_ADC_Start(&hadc1);  // 只需启动一次
}

ADC::~ADC() {
    //HAL_ADC_Stop(&hadc1); 
}

inline uint16_t ADC::read() {
    return readStatic(m_channel);
}

inline uint16_t ADC::readStatic(uint8_t channel) {
    HAL_ADC_Start(&hadc1);
    for (uint8_t i = 0; i < 2; i++) {
        if (HAL_ADC_PollForConversion(&hadc1, 10) == HAL_OK) {
            m_values[i] = HAL_ADC_GetValue(&hadc1);
        } else { 
            m_values[i] = 0;
        }
    }
    HAL_ADC_Stop(&hadc1); 
    return m_values[channel];
}