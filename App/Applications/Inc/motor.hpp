#ifndef MOTOR_HPP
#define MOTOR_HPP

#include "tb67h450.hpp"
#include "sin_map.h"

typedef struct
{
    uint16_t sinMapPtr;
    int16_t sinMapData;
    uint16_t dacValue12Bits;
} FastSinToDac_t;

class MOTOR {
public:
    inline void init() {
        m_motor_driver.init()
    }
    void SetFocCurrentVector(uint32_t _directionInCount, int32_t _current_mA);
    
private:
    TB67H450 m_motor_driver;
    FastSinToDac_t m_phaseB;
    FastSinToDac_t m_phaseA;
};

#endif // MOTOR_HPP