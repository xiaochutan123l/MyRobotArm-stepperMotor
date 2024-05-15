#include "timer.hpp"

void (*Timer::m_ticker100hz)(void*) = nullptr;
void (*Timer::m_ticker2hz)(void*) = nullptr;
void (*Timer::m_ticker20khz)(void*) = nullptr;
void* Timer::m_ticker100hzContext = nullptr;
void* Timer::m_ticker2hzContext = nullptr;
void* Timer::m_ticker20khzContext = nullptr;

Timer::Timer(uint8_t index) : m_index(index) {

}

void Timer::start() {
    switch (m_index) {
        case 1:
            HAL_TIM_Base_Start_IT(&htim1);
            break;
        case 2:
            HAL_TIM_Base_Start_IT(&htim2);
            break;
        case 4:
            HAL_TIM_Base_Start_IT(&htim4);
            break;
    }
}

void Timer::stop() {
    switch (m_index) {
        case 1:
            HAL_TIM_Base_Stop_IT(&htim1);
            break;
        case 2:
            HAL_TIM_Base_Stop_IT(&htim2);
            break;
        case 4:
            HAL_TIM_Base_Stop_IT(&htim4);
            break;
    }
}

void Timer::setCallback(void (*callback)(void*), void* context) {
    switch (m_index) {
        case 1:
            m_ticker100hz = callback;
            m_ticker100hzContext = context;
            break;
        case 2:
            m_ticker2hz = callback;
            m_ticker2hzContext = context;
            break;
        case 4:
            m_ticker20khz = callback;
            m_ticker20khzContext = context;
            break;
    }
}

extern "C" void Ticker100Hz() {
    Timer::ticker100hz();
}

extern "C" void Ticker2Hz() {
    Timer::ticker2hz();
}

extern "C" void Ticker20kHz() {
    Timer::ticker20khz();
}