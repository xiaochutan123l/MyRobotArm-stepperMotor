#include "timer.hpp"

Timer* timerInstances[TIMER_NUM] = {nullptr};

Timer::Timer(uint8_t index) : m_index(index) {

}

void Timer::init() {
    switch (m_index) {
        case TICKER_TIM1:
            m_htim = &htim1;
            timerInstances[TICKER_TIM1] = this;
            MX_TIM1_Init();
            break;
        case TICKER_TIM2:
            m_htim = &htim2;
            timerInstances[TICKER_TIM2] = this;
            MX_TIM2_Init();
            break;
        case TICKER_TIM4:
            m_htim = &htim4;
            timerInstances[TICKER_TIM4] = this;
            MX_TIM4_Init();
            break;
    }
}

void Timer::start() {
    if (m_htim != nullptr && timerInstances[m_index] != nullptr) {
        HAL_TIM_Base_Start_IT(m_htim);
    }
}

void Timer::stop() {
    if (m_htim && timerInstances[m_index]) {
        HAL_TIM_Base_Stop_IT(m_htim);
    }
}

void Timer::setCallback(void (*callback)(void*), void* context) {
    m_tickCallback = callback;
    m_tickerContext = context;
}

extern "C" void TickerTim1() {
    timerInstances[TICKER_TIM1]->tick();
}

extern "C" void TickerTim2() {
    timerInstances[TICKER_TIM2]->tick();
}

extern "C" void TickerTim4() {
    timerInstances[TICKER_TIM4]->tick();
}