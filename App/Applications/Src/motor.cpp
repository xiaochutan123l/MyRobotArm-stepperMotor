

void MOTOR::SetFocCurrentVector(uint32_t _directionInCount, int32_t _current_mA)
{
    phaseB.sinMapPtr = (_directionInCount) & (0x000003FF);// 限制在1023内
    phaseA.sinMapPtr = (phaseB.sinMapPtr + (256)) & (0x000003FF);// B相延后四分之一周期

    phaseA.sinMapData = sin_pi_m2[phaseA.sinMapPtr];// 取对应sin值
    phaseB.sinMapData = sin_pi_m2[phaseB.sinMapPtr];

    uint32_t dac_reg = fabs(_current_mA);// 绝对值
    dac_reg = (uint32_t)(dac_reg * 5083) >> 12; // 3300转4095
    dac_reg = dac_reg & (0x00000FFF);
    phaseA.dacValue12Bits =
        (uint32_t)(dac_reg * fabs(phaseA.sinMapData)) >> sin_pi_m2_dpiybit;// 由实际细分数得到实际电流（对标4095）
    phaseB.dacValue12Bits =
        (uint32_t)(dac_reg * fabs(phaseB.sinMapData)) >> sin_pi_m2_dpiybit;

    // SetTwoCoilsCurrent(phaseA.dacValue12Bits, phaseB.dacValue12Bits);
    //DacOutputVoltage(phaseA.dacValue12Bits, phaseB.dacValue12Bits);
    m_motor_driver.setVrefA(phaseA.dacValue12Bits >> 2);
    m_motor_driver.setVrefB(phaseB.dacValue12Bits >> 2);
    if (phaseA.sinMapData > 0)
        m_motor_driver.setOutptA(true, false);
    else if (phaseA.sinMapData < 0)
        m_motor_driver.setOutptA(false, true);
    else
        m_motor_driver.setOutptA(true, true);

    if (phaseB.sinMapData > 0)
        m_motor_driver.setOutptB(true, false);
    else if (phaseB.sinMapData < 0)
        m_motor_driver.setOutptB(false, true);
    else
        m_motor_driver.setOutptB(true, true);
}