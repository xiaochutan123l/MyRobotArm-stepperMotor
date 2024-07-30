#include "pid.hpp"

/**
  * @brief  参数配置
  * @param  _k
  * @retval NULL
**/
void Pid::SetKP(int32_t _k)
{
	if(_k <= 1024){		
		m_kp = _k;		
		m_valid_kp = true;		
	}
	else{															
		m_valid_kp = false;		
	}
}

/**
  * @brief  参数配置
  * @param  _k
  * @retval NULL
**/
void Pid::SetKI(int32_t _k)
{
	if(_k <= 1024){		
		m_ki = _k;		
		m_valid_ki = true;		
	}
	else{															
		m_valid_ki = false;		
	}
}

/**
  * @brief  参数配置
  * @param  _k
  * @retval NULL
**/
void Pid::SetKD(int32_t _k)
{
	if(_k <= 1024){		
		m_kd = _k;		
		m_valid_kd = true;		
	}
	else{															
		m_valid_kd = false;		
	}
}

/**
  * @brief  PID参数恢复
  * @param  NULL
  * @retval NULL
**/
void Pid::Set_Default(void)
{
	SetKP(De_PID_KP);
	SetKI(De_PID_KI);
	SetKD(De_PID_KD);
}

/**
  * @brief  控制器PID初始化
  * @param  NULL
  * @retval NULL
**/
void Pid::Init(void)
{
	//前置配置无效时,加载默认配置
	if(!m_valid_kp)				{	SetKP(De_PID_KP);		}
	if(!m_valid_ki)				{	SetKI(De_PID_KI);		}
	if(!m_valid_kd)				{	SetKD(De_PID_KD);		}
	
	//控制参数
	m_v_error = 0;	
	m_v_error_last = 0;
	m_op = 0;				
	m_oi = 0;			
	m_od = 0;	
	m_i_mut = 0;		
	m_i_dec = 0;
	m_out = 0;
}

int32_t Pid::CalcOutput(int32_t target_speed, int32_t current_speed, int32_t max_oi) {
	//误差
	m_v_error_last = m_v_error;
	//速度误差
	m_v_error = target_speed - current_speed;	
	if(m_v_error > ( 1024 * 1024))	m_v_error = ( 1024 * 1024);
	if(m_v_error < (-1024 * 1024))	m_v_error = (-1024 * 1024);

	//op输出
	m_op = ((m_kp) * (m_v_error));

	//oi输出
	m_i_mut += ((m_ki) * (m_v_error));
	m_i_dec  = (m_i_mut >> 10);
	m_i_mut -= (m_i_dec << 10);
	m_oi    += (m_i_dec);
	if(m_oi > (max_oi << 10))	{
		//限制为额定电流 * 1024
		m_oi = max_oi << 10;
	}	
	else if(m_oi < (-(max_oi << 10))) {
		//限制为额定电流 * 1024
		m_oi = -(max_oi << 10);
	}	

	//od输出
	m_od = (m_kd) * (m_v_error - m_v_error_last);

	//综合输出计算
	m_out = (m_op + m_oi + m_od) >> 10;
	if(m_out > max_oi)	{
		m_out =  max_oi;
	}
	else if(m_out < -max_oi) {
		m_out = -max_oi;
	}

	return m_out;
}