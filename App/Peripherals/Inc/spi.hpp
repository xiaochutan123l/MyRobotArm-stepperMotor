#ifndef SPI_HPP
#define SPI_HPP

#include "spi.h"
#include "peripheral_config.h"

class SPI {
public:
    SPI();
    inline uint16_t read(uint16_t cmd);
    
    static bool m_isInitialized;
    uint16_t m_data_rx;
};

#endif // SPI_HPP