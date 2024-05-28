#include "soft_eeprom.hpp"

Soft_EEPROM::Soft_EEPROM(uint8_t type) {
    switch(type) {
        case USER_DATA:
            m_stockpile = &stockpile_data;
            break;
        case CALI_DATA:
            m_stockpile = &stockpile_quick_cali;
            break;
    }
}