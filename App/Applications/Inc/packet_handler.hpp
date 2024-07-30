#ifndef PACKET_HANDLER_HPP
#define PACKET_HANDLER_HPP

#include <stdint.h>
#include "packet.h"
#include "string.h"
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
        if (IS_VALID_PACKET(data)) {
            memcpy(&m_packet, data, PACKET_LEN);
            m_received = true;
        }
    };

    inline Packet& getPacket() {
        return m_packet;
    }

    static Packet m_packet;
    static bool m_received;
};

#endif // PACKET_HANDLER_HPP