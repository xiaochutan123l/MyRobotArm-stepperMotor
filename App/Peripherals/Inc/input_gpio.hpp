#ifndef INPUT_GPIO_HPP
#define INPUT_GPIO_HPP

#include "gpio.h"
#include "peripheral_config.h"

class Input_GPIO {
public:
    Input_GPIO(uint16_t gpio_name);
    void init();
    inline bool readPin() {
        return (HAL_GPIO_ReadPin(m_gpio_group, m_gpio_pin) == GPIO_PIN_SET) ? true : false;
    };

    GPIO_TypeDef *m_gpio_group;
    GPIO_PinState m_pin_state;
    uint16_t m_gpio_pin;
    static bool m_isInitialized;
};

#endif // INPUT_GPIO_HPP