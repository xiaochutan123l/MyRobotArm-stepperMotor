#ifndef TIMEER_HPP
#define TIMEER_HPP

#include "tim.h"
#include "peripheral_config.h"

class Timer {
public:
    Timer(uint8_t index);
    void start();
    void stop();
    void setCallback(void (*callback)(void*), void* context);

    static inline void ticker100hz() {
        if (m_ticker100hz) {
        m_ticker100hz(m_ticker100hzContext);
    }
    };
    static inline void ticker2hz() {
        if (m_ticker2hz) {
        m_ticker2hz(m_ticker2hzContext);
    }
    };
    static inline void ticker20khz() {
        if (m_ticker20khz) {
        m_ticker20khz(m_ticker20khzContext);
    }
    };

private:
    uint8_t m_index;
    static void (*m_ticker100hz)(void*);
    static void (*m_ticker2hz)(void*);
    static void (*m_ticker20khz)(void*);
    static void* m_ticker100hzContext;
    static void* m_ticker2hzContext;
    static void* m_ticker20khzContext;

};

#endif // TIMEER_HPP