#ifndef ENCODER_HPP
#define ENCODER_HPP

#include "mt6818.hpp"

class Encoder {
public:
    inline void init() {
        m_encoder_sensor.init();
    }

    inline uint8_t updateAngle() {
        m_encoder_sensor.readAngle(m_angleData);
        // TODO: rectify the angle data.
        return m_angleData.data;
    }
    
    
private:
    MT6818 m_encoder_sensor;
    spiRawData_t m_angleData;
};

#endif // ENCODER_HPP