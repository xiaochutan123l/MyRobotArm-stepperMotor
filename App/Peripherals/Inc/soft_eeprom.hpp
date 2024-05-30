#ifndef SOFT_EEPROM_HPP
#define SOFT_EEPROM_HPP

#include "stockpile_f103cb.h"

#define USER_DATA 0
#define CALI_DATA 1

#define CALI_DATA_ADDR STOCKPILE_APP_CALI_ADDR

class Soft_EEPROM {
public: 
    Soft_EEPROM(uint8_t type);
    //Flash数据清空
    inline void Empty() {
        Stockpile_Flash_Data_Empty(m_stockpile);
    }	
    //Flash数据开始写入	
    inline void Begin() {
        Stockpile_Flash_Data_Begin(m_stockpile);
    }	
    //Flash数据结束写入	
    inline void End() {
        Stockpile_Flash_Data_End();
    }
    //Flash设置写地址			
    inline void Set_Write_Add(uint32_t write_add) {
        Stockpile_Flash_Data_Set_Write_Add(m_stockpile, write_add);
    }
    //Flash_16位数据写入				
    inline void Write_Data16_Append(uint16_t *data, uint32_t num) {
        Stockpile_Flash_Data_Write_Data16(m_stockpile, data, num);
    }
    //Flash_32位数据写入
    inline void Write_Data32_Append(uint32_t *data, uint32_t num) {
        Stockpile_Flash_Data_Write_Data32(m_stockpile, data, num);
    }	
    //Flash_64位数据写入
    inline void Write_Data64_Append(uint64_t *data, uint32_t num) {
        Stockpile_Flash_Data_Write_Data64(m_stockpile, data, num);
    }
private:
    Stockpile_FLASH_Typedef *m_stockpile;
};

#endif // SOFT_EEPROM_HPP