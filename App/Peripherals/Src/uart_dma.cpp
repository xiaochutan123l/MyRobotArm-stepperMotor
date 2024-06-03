#include "uart_dma.hpp"
#include <string.h>

// For old lib without set counter function.
#ifndef __HAL_DMA_SET_COUNTER
#define __HAL_DMA_SET_COUNTER(hdma, counter) ((hdma)->Instance->CNDTR = (counter))
#endif

volatile bool UartDMA::m_rxComplete = true;
volatile bool UartDMA::m_txComplete = true;

uint8_t UartDMA::m_recv_buf[RECV_BUF_SIZE] = {0};
uint8_t UartDMA::m_recv_data_buf[RECV_BUF_SIZE] = {0};

IDLERxCallback UartDMA::m_idle_rx_cb = nullptr;

UartDMA::UartDMA() : m_huart(&huart1), m_hdma_tx(&hdma_usart1_rx), m_hdma_rx(&hdma_usart1_tx)
{
}

void UartDMA::init() {
    //printf("dma init\n");
    if (!m_isInited) {
        DMA::init();
        MX_USART1_UART_Init();

        // 启用空闲中断
        __HAL_UART_ENABLE_IT(&huart1, UART_IT_IDLE);

        // 启动DMA接收
        HAL_UART_Receive_DMA(m_huart, m_recv_buf, RECV_BUF_SIZE);
    }
}

void UartDMA::transmit(uint8_t *data, uint16_t size)
{
    //printf("tx start\n");
    m_txComplete = false;
    HAL_UART_Transmit_DMA(m_huart, data, size);
}

// // depricated
// void UartDMA::receive(uint8_t *data, uint16_t size)
// {
//     // m_rxBuffer = data;
//     // m_rxBufferSize = size;
//     m_rxComplete = false;
//     HAL_UART_Receive_DMA(m_huart, data, size);
// }

// 应该考虑istxcomplete这个问题。可能需要加多个buffer
int UartDMA::printf(const char* format, ...) {
    va_list args;
    va_start(args, format);
    int length = vsnprintf(m_send_buf, SEND_BUF_SIZE, format, args);
    va_end(args);

    if (length > 0) {
        transmit((uint8_t*)m_send_buf, length);
    }

    return length;
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

void UartDMA::setIDLECallback(IDLERxCallback callback)
{
    m_idle_rx_cb = callback;
}

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
    // //UartDMA::RxCpltCallback(huart);
    if (huart->Instance == USART1) // 确认是我们使用的 UART 实例
    {
        UartDMA::onRxComplete();
    }
}

// uart 闲时中断
extern "C" void HAL_UART_IDLECallback(UART_HandleTypeDef *huart)
{
    if (huart->Instance == USART1)
    {
        // 获取接收到的数据长度
        uint16_t received_length = RECV_BUF_SIZE - __HAL_DMA_GET_COUNTER(huart->hdmarx);
        
        // 将接收到的数据拷贝到dataBuffer
        memcpy(UartDMA::m_recv_data_buf, UartDMA::m_recv_buf, received_length);

        // 处理接收到的数据
        if (UartDMA::m_idle_rx_cb) {
            UartDMA::m_idle_rx_cb(UartDMA::m_recv_data_buf, received_length);
        }

        // 重启DMA接收
        __HAL_DMA_DISABLE(huart->hdmarx);
        // GPT说的有问题
        __HAL_DMA_SET_COUNTER(huart->hdmarx, RECV_BUF_SIZE);
        __HAL_DMA_ENABLE(huart->hdmarx);
    }
}

// void processReceivedData(uint8_t* data, uint16_t length)
// {
//     // 处理接收到的数据，例如存储或解析
//     for (uint16_t i = 0; i < length; ++i)
//     {
//         printf("%c", data[i]);
//     }
//     printf("\n");
// }