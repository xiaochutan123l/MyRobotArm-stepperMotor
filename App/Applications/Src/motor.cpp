#include "motor.hpp"

void Motor::SetFocCurrentVector(uint32_t _directionInCount, int32_t _current_mA)
{
    m_phaseB.sinMapPtr = (_directionInCount) & (0x000003FF);// 限制在1023内
    m_phaseA.sinMapPtr = (m_phaseB.sinMapPtr + (256)) & (0x000003FF);// B相延后四分之一周期

    m_phaseA.sinMapData = sin_pi_m2[m_phaseA.sinMapPtr];// 取对应sin值
    m_phaseB.sinMapData = sin_pi_m2[m_phaseB.sinMapPtr];

    uint32_t dac_reg = fabs(_current_mA);// 绝对值
    dac_reg = (uint32_t)(dac_reg * 5083) >> 12; // 3300转4095
    dac_reg = dac_reg & (0x00000FFF);
    m_phaseA.dacValue12Bits =
        (uint32_t)(dac_reg * fabs(m_phaseA.sinMapData)) >> sin_pi_m2_dpiybit;// 由实际细分数得到实际电流（对标4095）
    m_phaseB.dacValue12Bits =
        (uint32_t)(dac_reg * fabs(m_phaseB.sinMapData)) >> sin_pi_m2_dpiybit;

    // SetTwoCoilsCurrent(m_phaseA.dacValue12Bits, m_phaseB.dacValue12Bits);
    //DacOutputVoltage(m_phaseA.dacValue12Bits, m_phaseB.dacValue12Bits);
    m_motor_driver.setVrefA(m_phaseA.dacValue12Bits >> 2);
    m_motor_driver.setVrefB(m_phaseB.dacValue12Bits >> 2);
    if (m_phaseA.sinMapData > 0)
        m_motor_driver.setOutptA(true, false);
    else if (m_phaseA.sinMapData < 0)
        m_motor_driver.setOutptA(false, true);
    else
        m_motor_driver.setOutptA(true, true);

    if (m_phaseB.sinMapData > 0)
        m_motor_driver.setOutptB(true, false);
    else if (m_phaseB.sinMapData < 0)
        m_motor_driver.setOutptB(false, true);
    else
        m_motor_driver.setOutptB(true, true);
}