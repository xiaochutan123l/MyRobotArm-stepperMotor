#ifndef GENERIC_GPIO_HPP
#define GENERIC_GPIO_HPP

#include "gpio.h"
#include "peripheral_config.h"



class Generic_GPIO {
public:
    Generic_GPIO(uint8_t gpio_name);
    inline void setLow();
    inline void setHigh();

    GPIO_TypeDef *m_gpio_group;
    uint16_t m_gpio_pin;
    static bool m_isInitialized;
};

#endif // GENERIC_GPIO_HPP