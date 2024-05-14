#ifndef UARTDMA_HPP
#define UARTDMA_HPP

#include "main.h"
#include "usart.h"
#include <cstdint>

extern UART_HandleTypeDef huart1;

extern DMA_HandleTypeDef hdma_usart1_rx;
extern DMA_HandleTypeDef hdma_usart1_tx;

class UartDMA
{
public:
    UartDMA();
    void transmit(uint8_t *data, uint16_t size);
    void receive(uint8_t *data, uint16_t size);

    inline void onTxComplete();
    inline void onRxComplete();

    static inline void TxCpltCallback(UART_HandleTypeDef *huart);
    static inline void RxCpltCallback(UART_HandleTypeDef *huart);

private:
    UART_HandleTypeDef *m_huart;
    DMA_HandleTypeDef *m_hdma_tx;
    DMA_HandleTypeDef *m_hdma_rx;

    uint8_t *m_rxBuffer;
    uint16_t m_rxBufferSize;
    volatile bool m_rxComplete;
    volatile bool m_txComplete;
};

#endif // UARTDMA_HPP