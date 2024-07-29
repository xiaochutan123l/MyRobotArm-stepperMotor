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

void Soft_EEPROM::Write_Data32(void *data, uint32_t length) {
    // 擦除存储数据的Flash页
    Empty();
    Begin();
    uint32_t *dataPtr = (uint32_t *)data;
    Write_Data32_Append(dataPtr, length / sizeof(uint32_t));
    End();
}