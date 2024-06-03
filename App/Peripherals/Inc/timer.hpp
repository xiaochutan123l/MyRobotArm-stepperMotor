#ifndef TIMEER_HPP
#define TIMEER_HPP

#include "tim.h"
#include "peripheral_config.h"

#include "utils.h"

class Timer {
public:
    Timer(uint8_t index);
    void init();
    void start();
    void stop();
    void setCallback(void (*callback)());

    inline void tick() {
        if (m_tickCallback) {
            m_tickCallback();
        }
    };

private:
    TIM_HandleTypeDef *m_htim;
    uint8_t m_index;
    void (*m_tickCallback)();

};

#endif // TIMEER_HPP