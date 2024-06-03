#ifndef COM_UART_HPP
#define COM_UART_HPP

#include <stdint.h>
#include "packet_handler.hpp"
#include "uart_dma.hpp"

class ComUart : public PacketHandler, UartDMA {
    void init() override {
        UartDMA::init();
        setIDLECallback(packet_process);
    };
    void setIDLECallback(IDLERxCallback callback) {
        m_uart.setIDLECallback(callback);
    }
};

#endif // COM_UART_HPP