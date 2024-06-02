#ifndef PID_HPP
#define PID_HPP

#include <cstdint>
#include "control_config.h"

#define De_PID_KP	5		//默认KP
#define De_PID_KI	30	//默认KI
#define De_PID_KD	0		//默认KD

class Pid {
public:

    //参数配置
    void SetKP(uint16_t _k);		//KP参数配置
    void SetKI(uint16_t _k);		//KI参数配置
    void SetKD(uint16_t _k);		//KD参数配置
    void Set_Default(void);			//PID参数恢复
    //初始化
    void Init(void);
    int32_t CalcOutput(int32_t target_speed, int32_t current_speed, int32_t max_oi=Current_Rated_Current);
    void Clear_Integral() {
        m_i_mut = 0;
        m_i_dec = 0;
        m_oi = 0;
    };
private:
    //参数有效标志
    bool m_valid_kp;
    bool m_valid_ki; 
    bool m_valid_kd;	
    //参数
	int32_t	m_kp;
    int32_t m_ki; 
    int32_t m_kd;		
	//控制参数
    //误差记录	
    int32_t m_v_error;
    int32_t m_v_error_last;	
    //输出
	int32_t	m_op;
    int32_t m_oi;
    int32_t m_od;		
    //小积分处理					
	int32_t	m_i_mut;
    int32_t m_i_dec;						
	int32_t	m_out;	
};

#endif // PID_HPP