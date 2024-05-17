#ifndef MT6816_HPP
#define MT6816_HPP

#include "spi.hpp"
//#include "peripheral_config.h"

typedef struct {
    uint16_t data;      // 14bits rawAngle in rawData
    bool noMagFlag;
    bool checksumFlag;
} spiRawData_t;

class MT6816 {
public:
    void init();
    bool readAngle(spiRawData_t &rawData);
    
private:
    SPI m_spi;
    uint16_t m_dataTx[2];
    uint16_t m_dataRx[2];
    uint8_t m_hCount;
};

#endif // MT6816_HPP