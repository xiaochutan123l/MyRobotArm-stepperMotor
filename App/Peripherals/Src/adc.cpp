#include "adc.hpp"

bool ADC::m_isInitialized = false;

ADC::ADC(uint32_t channel) : m_channel(channel) {
    // if (!m_isInitialized) {
    //     MX_ADC1_Init();
    //     m_isInitialized = true;
    // }
    //HAL_ADC_Start(&hadc1);  // 只需启动一次
}

void ADC::init() {
    if (!m_isInitialized) {
    MX_ADC1_Init();
    m_isInitialized = true;
}
}