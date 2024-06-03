#ifndef PACKET_HANDLER_HPP
#define PACKET_HANDLER_HPP

#include <stdint.h>

struct Packet
{
    /* data */
    uint8_t cmd_type;
    uint8_t data_len;
    uint8_t *data;
};


class PacketHandler
{
public:
    inline bool is_new_packet_received() {
        return m_received;
    };
    inline void set_packet_processed() {
        m_received = false;
    };
    inline void packet_process(uint8_t *data, uint16_t size) {
        m_packet.cmd_type = data[0];
        m_packet.data_len = data[1];
        m_packet.data = data + 2;
        m_received = true;
    };

    inline uint8_t getCmdType () {
        return m_packet.cmd_type;
    };

    inline uint8_t getDataLen () {
        return m_packet.data_len;
    };

    inline uint8_t getData () {
        return m_packet.data;
    };

    Packet m_packet;
    bool m_received = false;
};

#endif // PACKET_HANDLER_HPP