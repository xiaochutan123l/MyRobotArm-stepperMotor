#include "spi.hpp"

static bool m_isInitialized = false;

SPI::SPI() {
    if (!m_isInitialized) {
        MX_SPI1_Init();
        m_isInitialized = true;
    }
}

SPI::inline uint16_t read(uint16_t cmd) {
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_RESET);
    //GPIOA->BRR = GPIO_PIN_4;
    HAL_SPI_TransmitReceive(&hspi1, (uint8_t*) &cmd, (uint8_t*) &m_data_rx, 1, HAL_MAX_DELAY);
    //GPIOA->BSRR = GPIO_PIN_4;
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_SET);
    return m_data_rx;
}