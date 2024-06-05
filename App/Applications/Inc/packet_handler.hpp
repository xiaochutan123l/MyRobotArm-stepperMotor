#ifndef PACKET_HANDLER_HPP
#define PACKET_HANDLER_HPP

#include <stdint.h>
//#include "utils.h"
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
    inline static void packet_process(uint8_t *data, uint16_t size) {
        m_packet.cmd_type = data[0];
        m_packet.data_len = data[1];
        m_packet.data = data + 2;
        m_received = true;
        //printf("set true\n");
    };

    inline uint8_t getCmdType () {
        return m_packet.cmd_type;
    };

    inline uint8_t getDataLen () {
        return m_packet.data_len;
    };

    inline uint8_t* getData () {
        return m_packet.data;
    };

    static Packet m_packet;
    static bool m_received;
};

#endif // PACKET_HANDLER_HPP