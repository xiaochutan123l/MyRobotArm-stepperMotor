#include "dma.hpp"

bool DMA::m_isInitialized = false;

DMA::DMA() {
    if (!m_isInitialized) {
        MX_DMA_Init();
        m_isInitialized = true;
    }
}