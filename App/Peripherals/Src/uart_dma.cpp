#include "uart_dma.hpp"

UartDMA::UartDMA() : m_huart(&huart1), m_hdma_tx(&hdma_usart1_rx), m_hdma_rx(&hdma_usart1_tx)
{
    MX_USART1_UART_Init();
    // start dma for incomming uart message.
    HAL_UART_Receive_DMA(m_huart, data, size);
}

void UartDMA::transmit(uint8_t *data, uint16_t size)
{
    HAL_UART_Transmit_DMA(m_huart, data, size);
}

void UartDMA::receive(uint8_t *data, uint16_t size)
{
    m_rxBuffer = data;
    m_rxBufferSize = size;
    m_rxComplete = false;
    HAL_UART_Receive_DMA(m_huart, data, size);
}

inline void UartDMA::onTxComplete()
{
    m_txComplete = true;
    // 传输完成处理
}

inline void UartDMA::onRxComplete()
{
    m_rxComplete = true;
    // 接收完成处理
}

// 静态回调函数
inline void UartDMA::TxCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart->Instance == USART1) // 确认是我们使用的 UART 实例
    {
        UartDMA *instance = reinterpret_cast<UartDMA*>(huart->pUserData);
        if (instance)
        {
            instance->onTxComplete();
        }
    }
}

inline void UartDMA::RxCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart->Instance == USART1) // 确认是我们使用的 UART 实例
    {
        UartDMA *instance = reinterpret_cast<UartDMA*>(huart->pUserData);
        if (instance)
        {
            instance->onRxComplete();
        }
    }
}

// 定义回调函数
extern "C" void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
    UartDMA::TxCpltCallback(huart);
}

extern "C" void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    UartDMA::RxCpltCallback(huart);
}
