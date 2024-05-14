#ifndef ADC_HPP
#define ADC_HPP

#include "adc.h"
#include "peripheral_config.h"

class ADC {
public:
    ADC(uint8_t channel);
    ~ADC( );
    
    inline uint16_t read();
    uint8_t m_channel;
    static uint16_t m_values[2];
    static bool m_isInitialized;

private:
    static inline uint16_t readStatic(uint8_t channel);

};

#endif // ADC_HPP