#include "dce.hpp"

/**
  * @brief  参数配置
  * @param  _k
  * @retval NULL
**/
void Dce::SetKP(uint16_t _k)
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
void Dce::SetKI(uint16_t _k)
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
void Dce::SetKV(uint16_t _k)
{
	if(_k <= 1024){		
        m_kv = _k;		
        m_valid_kv = true;		
    }
	else{															
        m_valid_kv = false;		
    }
}

/**
  * @brief  参数配置
  * @param  _k
  * @retval NULL
**/
void Dce::SetKD(uint16_t _k)
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
  * @brief  DCE参数恢复
  * @param  NULL
  * @retval NULL
**/
void Dce::Set_Default(void)
{
	SetKP(De_DCE_KP);
	SetKI(De_DCE_KI);
	SetKV(De_DCE_KV);
	SetKD(De_DCE_KD);
}

/**
  * @brief  控制器DCE初始化
  * @param  NULL
  * @retval NULL
**/
void Dce::Init(void)
{
	//前置配置无效时,加载默认配置
	if(!m_valid_kp)				{	SetKP(De_DCE_KP);		}
	if(!m_valid_ki)				{	SetKI(De_DCE_KI);		}
	if(!m_valid_kv)				{	SetKV(De_DCE_KV);		}
	if(!m_valid_kd)				{	SetKD(De_DCE_KD);		}
	
	//控制参数(基本部分)
	m_p_error = 0;	
    m_v_error = 0;
	m_op = 0;				
    m_oi = 0;			
    m_od = 0;	
	m_i_mut = 0;		
    m_i_dec = 0;
	m_out = 0;
}

/**
  * @brief  DCE电流控制
  * @param  _location 控制位置
  * @param  _speed    控制速度
  * @retval NULL
**/
int32_t Dce::CalcOutput(int32_t target_location, int32_t target_speed, int32_t current_location, int32_t current_speed, int32_t max_oi)
{
	//误差
	m_p_error = target_location - current_location;
	m_v_error = (target_speed - current_speed) >> 7;	//速度误差缩小至1/128
	if(m_p_error > ( 3200))	m_p_error = ( 3200);				//限制位置误差在1/16圈内(51200/16)
	if(m_p_error < (-3200))	m_p_error = (-3200);
	if(m_v_error > ( 4000))	m_v_error = ( 4000);				//限制速度误差在10r/s内(51200*10/128)
	if(m_v_error < (-4000))	m_v_error = (-4000);

	//op输出计算
	m_op     = ((m_kp) * (m_p_error));

	//oi输出计算
	m_i_mut += ((m_ki) * (m_p_error));
	m_i_mut += ((m_kv) * (m_v_error));
	m_i_dec  = (m_i_mut >> 7);
	m_i_mut -= (m_i_dec << 7);
	m_oi    += (m_i_dec);
	if(m_oi > ( max_oi << 10 ))	{
        //限制为额定电流 * 1024
        m_oi = max_oi << 10 ;	
    }
	else if(m_oi < (-(max_oi << 10))) {	
        //限制为额定电流 * 1024
        m_oi = (-(max_oi << 10));
    }	

	//od输出计算
	m_od = ((m_kd) * (m_v_error));

	//综合输出计算
	m_out = (m_op + m_oi + m_od) >> 10;
	if(m_out > max_oi) {
        m_out =  max_oi;
    }
	else if(m_out < -max_oi) {
        m_out = -max_oi;
    }

	//输出FOC电流
	return m_out;
}
