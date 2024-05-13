#ifndef ADC_HPP
#define ADC_HPP

#include "adc.h"
#include "peripheral_config.h"

class ADC {
public:
    ADC(uint8_t channel);
    ~ADC( );
    static inline uint16_t read();

    static uint16_t m_values[2];
    static bool m_isInitialized;

};

#endif // ADC_HPP