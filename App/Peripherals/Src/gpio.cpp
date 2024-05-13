#include "generic_gpio.hpp"

static bool m_isInitialized = false;

Generic_GPIO::Generic_GPIO(uint8_t gpio_name) {
    if (!m_isInitialized) {
        MX_GPIO_Init();
        m_isInitialized = true;
    }
    switch(gpio_name) {
        case AIN1_Pin:
            m_gpio_group = GPIOB;
            m_gpio_pin = GPIO_PIN_6;
            break;
        case AIN2_Pin:
            m_gpio_group = GPIOB;
            m_gpio_pin = GPIO_PIN_7;
            break;
        case BIN1_Pin:
            m_gpio_group = GPIOB;
            m_gpio_pin = GPIO_PIN_8;
            break;
        case BIN2_Pin:
            m_gpio_group = GPIOB;
            m_gpio_pin = GPIO_PIN_9;
            break;
    }
}

inline void Generic_GPIO::setHigh() {
    HAL_GPIO_WritePin(m_gpio_group, m_gpio_pin, GPIO_PIN_SET);
}

inline void Generic_GPIO::setLow() {
    HAL_GPIO_WritePin(m_gpio_group, m_gpio_pin, GPIO_PIN_RESET);
}