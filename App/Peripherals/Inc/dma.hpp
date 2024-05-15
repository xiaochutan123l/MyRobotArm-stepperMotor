#ifndef DMA_HPP
#define DMA_HPP

#include "dma.h"
#include "peripheral_config.h"

class DMA {
public:
    //DMA();
    void init();
    static bool m_isInitialized;
};

#endif // DMA_HPP