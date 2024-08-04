#include <stdint.h>
#include "input_gpio.hpp"

//typedef void (*ButtonCallback)(void);

class Button {
public:
    Button(uint8_t gpio_name)
        : m_gpio(gpio_name), m_isPressed(false) {}

    void init() {
        m_gpio.init();
    }

    // void setShortPressCallback(ButtonCallback callback) {
    //     m_shortPressCallback = callback;
    // }

    // void setLongPressCallback(ButtonCallback callback) {
    //     m_longPressCallback = callback;
    // }

    // void tick() {}
    inline bool isPressed() {
        return m_gpio.readPin();
    }

private:
    Input_GPIO m_gpio;
    bool m_isPressed;
};
