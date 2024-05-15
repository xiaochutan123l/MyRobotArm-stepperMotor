#ifndef ADC_HPP
#define ADC_HPP

#include "adc.h"
#include "peripheral_config.h"

class ADC {
public:
    ADC(uint32_t channel);
    void init();
    inline uint16_t read() {
        return readStatic(m_channel);
    };
    uint32_t m_channel;
    static uint16_t m_values[2];
    static bool m_isInitialized;

private:
    static inline uint16_t readStatic(uint32_t channel) {
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
    };

};

#endif // ADC_HPP