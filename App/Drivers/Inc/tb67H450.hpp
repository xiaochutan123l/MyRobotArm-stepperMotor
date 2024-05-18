#ifndef TB67H450_HPP
#define TB67H450_HPP

#include "generic_gpio.hpp"
#include "pwm.hpp"

class TB67H450 {
public:
    TB67H450();
    void init();
    
    inline void setOutptA(bool out1, bool out2){
        out1? m_outputA1.setHigh() : m_outputA1.setLow();
        out2? m_outputA2.setHigh() : m_outputA2.setLow();
    };
    inline void setOutptB(bool out1, bool out2){
        out1? m_outputB1.setHigh() : m_outputB1.setLow();
        out2? m_outputB2.setHigh() : m_outputB2.setLow();
    };
    inline void setVrefA(uint16_t value){
        m_Vref_A.setVoltage(value);
    };
    inline void setVrefB(uint16_t value){
        m_Vref_B.setVoltage(value);
    };
private:
    Generic_GPIO m_outputA1;
    Generic_GPIO m_outputA2;
    Generic_GPIO m_outputB1;
    Generic_GPIO m_outputB2;
    PWM m_Vref_A;
    PWM m_Vref_B;
};

#endif // TB67H450_HPP