#include "generic_gpio.hpp"

bool Generic_GPIO::m_isInitialized = false;

Generic_GPIO::Generic_GPIO(uint16_t gpio_name) {
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

void Generic_GPIO::init() {
    if (!m_isInitialized) {
        MX_GPIO_Init();
        m_isInitialized = true;
    }
}