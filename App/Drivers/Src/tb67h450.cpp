#include "tb67h450.hpp"

TB67H450::TB67H450() : m_outputA1(AIN1_Pin), m_outputA2(AIN2_Pin), 
          m_outputB1(BIN1_Pin), m_outputB2(BIN2_Pin), 
          m_Vref_A(VREF_A), m_Vref_B(VREF_B){
}

void TB67H450::init() {
    m_outputA1.init();
    m_outputA2.init();
    m_outputB1.init();
    m_outputB2.init();
    m_Vref_A.init();
    m_Vref_B.init();
}