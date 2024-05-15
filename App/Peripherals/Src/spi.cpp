#include "spi.hpp"

bool SPI::m_isInitialized = false;

void SPI::init() {
    if (!m_isInitialized) {
        MX_SPI1_Init();
        m_isInitialized = true;
    }
}