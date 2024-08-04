#ifndef ENCODER_HPP
#define ENCODER_HPP

#include "mt6816.hpp"
#include "soft_eeprom.hpp"
#include "control_config.h"

class Encoder {
public:
    inline void init() {
        m_encoder_sensor.init();
        m_isInit = true;
        m_rect_valid = true;
        for(uint32_t i=0; i<(CALI_Encode_Res); i++){
            if(m_flash_cali_data[i] == 0xFFFF)
                m_rect_valid = false;
	    }
    }
    inline bool isInit() {
        return m_isInit;
    };
    inline uint16_t updateAngle() {
        m_encoder_sensor.readAngle(m_angleData);
        // TODO: rectify the angle data.
        return m_angleData.data;
    }
    inline uint16_t updateRectAngle() {
        return m_flash_cali_data[updateAngle()];
    };
    inline bool isRectValid() {return m_rect_valid;};
    inline void setRectValid(bool valid) {m_rect_valid = valid;};
    
    
private:
    MT6816 m_encoder_sensor;
    spiRawData_t m_angleData;
    uint16_t *m_flash_cali_data = (uint16_t*)CALI_DATA_ADDR;
    bool m_rect_valid;
    bool m_isInit = false;
};

#endif // ENCODER_HPP