#ifndef UARTDMA_HPP
#define UARTDMA_HPP

#include "main.h"
#include "usart.h"
#include "dma.hpp"
#include <cstdint>

#include "utils.h"

extern UART_HandleTypeDef huart1;

extern DMA_HandleTypeDef hdma_usart1_rx;
extern DMA_HandleTypeDef hdma_usart1_tx;

// currently UartDMA only work for UART1, 
// later may need to be reconstructed to adapt multiple uarts.

class UartDMA : DMA
{
public:
    UartDMA();
    void init();
    void transmit(uint8_t *data, uint16_t size);
    // Receive does not work yet.
    void receive(uint8_t *data, uint16_t size);

    static inline void onTxComplete();
    static inline void onRxComplete();

    volatile static bool m_rxComplete;
    volatile static bool m_txComplete;

private:
    // add callback later if specific requirements exist.
    // static inline void TxCpltCallback(UART_HandleTypeDef *huart);
    // static inline void RxCpltCallback(UART_HandleTypeDef *huart);

    UART_HandleTypeDef *m_huart;
    DMA_HandleTypeDef *m_hdma_tx;
    DMA_HandleTypeDef *m_hdma_rx;

    // uint8_t *m_rxBuffer;
    // uint16_t m_rxBufferSize;
};

#endif // UARTDMA_HPP