#ifndef SPI_HPP
#define SPI_HPP

#include "spi.h"
#include "peripheral_config.h"

class SPI {
public:
    // SPI();
    void init();
    inline uint16_t read(uint16_t cmd) {
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_RESET);
        //GPIOA->BRR = GPIO_PIN_4;
        HAL_SPI_TransmitReceive(&hspi1, (uint8_t*) &cmd, (uint8_t*) &m_data_rx, 1, HAL_MAX_DELAY);
        //GPIOA->BSRR = GPIO_PIN_4;
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_SET);
        return m_data_rx;
    };
    
    static bool m_isInitialized;
    uint16_t m_data_rx;
};

#endif // SPI_HPP