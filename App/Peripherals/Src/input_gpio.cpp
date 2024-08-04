#include "input_gpio.hpp"

bool Input_GPIO::m_isInitialized = false;

Input_GPIO::Input_GPIO(uint16_t gpio_name) {
    switch(gpio_name) {
        case BUTTON_1:
            m_gpio_group = GPIOB;
            m_gpio_pin = GPIO_PIN_1;
            break;
        case BUTTON_2:
            m_gpio_group = GPIOB;
            m_gpio_pin = GPIO_PIN_2;
            break;
    }
}

void Input_GPIO::init() {
    if (!m_isInitialized) {
        MX_GPIO_Input_Init();
        m_isInitialized = true;
    }
}