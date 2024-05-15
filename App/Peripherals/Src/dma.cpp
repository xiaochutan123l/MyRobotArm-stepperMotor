#include "dma.hpp"

bool DMA::m_isInitialized = false;

void DMA::init() {
    if (!m_isInitialized) {
        MX_DMA_Init();
        m_isInitialized = true;
    }
}