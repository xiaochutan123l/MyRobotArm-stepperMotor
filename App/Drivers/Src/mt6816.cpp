#include "mt6816.hpp"

void MT6816::init() {
    m_spi.init();
    m_dataTx[0] = (0x80 | 0x03) << 8;
    m_dataTx[1] = (0x80 | 0x04) << 8;
}

bool MT6816::readAngle(spiRawData_t &rawData) {
  for (uint8_t i = 0; i < 3; i++) {
    m_dataRx[0] = m_spi.read(m_dataTx[0]);
    m_dataRx[1] = m_spi.read(m_dataTx[1]);

    rawData.data = ((m_dataRx[0] & 0x00FF) << 8) | (m_dataRx[1] & 0x00FF);

    m_hCount = 0;
    for (uint8_t j = 0; j < 16; j++)
    {
        if (rawData.data & (0x0001 << j))
            m_hCount++;
    }
    if (m_hCount & 0x01)
    {
        rawData.checksumFlag = false;
    } else
    {
        rawData.checksumFlag = true;
        break;
    }
  }

  if (rawData.checksumFlag) {
    rawData.noMagFlag = (bool) (rawData.data & 0x0010);
    rawData.data = rawData.data >> 2;
  }
  else {
    return false;
  }
  return true;
}