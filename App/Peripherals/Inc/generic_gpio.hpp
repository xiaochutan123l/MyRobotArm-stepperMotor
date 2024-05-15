#ifndef GENERIC_GPIO_HPP
#define GENERIC_GPIO_HPP

#include "gpio.h"
#include "peripheral_config.h"



class Generic_GPIO {
public:
    Generic_GPIO(uint16_t gpio_name);
    void init();
    inline void setLow() {HAL_GPIO_WritePin(m_gpio_group, m_gpio_pin, GPIO_PIN_SET);};
    inline void setHigh() {HAL_GPIO_WritePin(m_gpio_group, m_gpio_pin, GPIO_PIN_RESET);};

    GPIO_TypeDef *m_gpio_group;
    uint16_t m_gpio_pin;
    static bool m_isInitialized;
};

#endif // GENERIC_GPIO_HPP