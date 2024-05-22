#include "uart_dma.hpp"

volatile bool UartDMA::m_rxComplete = true;
volatile bool UartDMA::m_txComplete = true;

UartDMA::UartDMA() : m_huart(&huart1), m_hdma_tx(&hdma_usart1_rx), m_hdma_rx(&hdma_usart1_tx)
{
}

void UartDMA::init() {
    //printf("dma init\n");
    DMA::init();
    MX_USART1_UART_Init();
}

void UartDMA::transmit(uint8_t *data, uint16_t size)
{
    //printf("tx start\n");
    m_txComplete = false;
    HAL_UART_Transmit_DMA(m_huart, data, size);
}

void UartDMA::receive(uint8_t *data, uint16_t size)
{
    // m_rxBuffer = data;
    // m_rxBufferSize = size;
    m_rxComplete = false;
    HAL_UART_Receive_DMA(m_huart, data, size);
}

inline void UartDMA::onTxComplete()
{
    //printf("set tx complete true\n");
    m_txComplete = true;
    // 传输完成处理
}

inline void UartDMA::onRxComplete()
{
    m_rxComplete = true;
    // 接收完成处理
}

// // 静态回调函数
// inline void UartDMA::TxCpltCallback(UART_HandleTypeDef *huart)
// {
//     if (huart->Instance == USART1) // 确认是我们使用的 UART 实例
//     {
//         UartDMA::onTxComplete();
//     }
// }

// inline void UartDMA::RxCpltCallback(UART_HandleTypeDef *huart)
// {
//     if (huart->Instance == USART1) // 确认是我们使用的 UART 实例
//     {
//         UartDMA::onRxComplete();
//     }
// }

// 定义回调函数
extern "C" void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
    //UartDMA::TxCpltCallback(huart);
    if (huart->Instance == USART1) // 确认是我们使用的 UART 实例
    {
        UartDMA::onTxComplete();
        //printf("tx complete callback;\n");
    }
}

extern "C" void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    //UartDMA::RxCpltCallback(huart);
    if (huart->Instance == USART1) // 确认是我们使用的 UART 实例
    {
        UartDMA::onRxComplete();
    }
}
