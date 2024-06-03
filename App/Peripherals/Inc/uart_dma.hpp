#ifndef UARTDMA_HPP
#define UARTDMA_HPP

#include "main.h"
#include "usart.h"
#include "dma.hpp"
#include <cstdint>

#include "utils.h"
#include <cstdarg>

#define SEND_BUF_SIZE 64
#define RECV_BUF_SIZE 64

extern UART_HandleTypeDef huart1;

extern DMA_HandleTypeDef hdma_usart1_rx;
extern DMA_HandleTypeDef hdma_usart1_tx;

typedef void (*IDLERxCallback)(uint8_t *data, uint16_t length);

// currently UartDMA only work for UART1, 
// later may need to be reconstructed to adapt multiple uarts.

// Limitation: 
// 1. actually should be singleton mode, only one instance valid,since some static members defined.
//    but if later requires of course can be adapted by allocate individual buffer for each instance.

class UartDMA : DMA
{
public:
    UartDMA();
    void init();
    void transmit(uint8_t *data, uint16_t size);
    // Receive does not work yet.
    // void receive(uint8_t *data, uint16_t size);
    int printf(const char* format, ...);

    void setIDLECallback(IDLERxCallback callback);

    static inline void onTxComplete();
    static inline void onRxComplete();

    volatile static bool m_rxComplete;
    volatile static bool m_txComplete;

    static uint8_t m_recv_buf[RECV_BUF_SIZE];
    static uint8_t m_recv_data_buf[RECV_BUF_SIZE];
    static IDLERxCallback m_idle_rx_cb;
private:
    UART_HandleTypeDef *m_huart;
    DMA_HandleTypeDef *m_hdma_tx;
    DMA_HandleTypeDef *m_hdma_rx;
    char m_send_buf[SEND_BUF_SIZE];
    bool m_isInited = false;
};

#endif // UARTDMA_HPP