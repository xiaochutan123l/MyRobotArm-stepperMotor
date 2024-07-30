#ifndef DCE_HPP
#define DCE_HPP

#include <cstdint>
#include "control_config.h"
#define De_DCE_KP	200		//默认KP
#define De_DCE_KI	80		//默认KI
#define De_DCE_KV	300		//默认KIV
#define De_DCE_KD	250		//默认KD

class Dce {
public:

    //参数配置
    void SetKP(int32_t _k);		
    void SetKI(int32_t _k);		
    void SetKV(int32_t _k);		
    void SetKD(int32_t _k);		
    //DCE参数恢复
    void Set_Default(void);			
    //初始化
    void Init(void);
    int32_t CalcOutput(int32_t target_location, int32_t target_speed, int32_t current_location, int32_t current_speed, int32_t max_oi=Current_Rated_Current);
    void Clear_Integral() {
        m_i_mut = 0;
        m_i_dec = 0;
        m_oi = 0;
    };
private:
    //参数有效标志
    bool	m_valid_kp;
    bool    m_valid_ki;
    bool    m_valid_kv;
    bool    m_valid_kd;	
    //参数
	int32_t	m_kp;
    int32_t	m_ki;
    int32_t	m_kv;
    int32_t	m_kd;		
	//控制参数(基本部分)
    //误差记录
	int32_t	m_p_error;
    int32_t	m_v_error;	
    //输出	
	int32_t	m_op;
    int32_t	m_oi;
    int32_t	m_od;			
    //小积分处理		
	int32_t	m_i_mut;
    int32_t	m_i_dec;	
    //输出			
	int32_t	m_out;								
};

#endif // DCE_HPP