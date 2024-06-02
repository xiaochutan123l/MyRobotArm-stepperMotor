/******
	************************************************************************
	******
	** @project : XDrive_Step
	** @brief   : Stepper motor with multi-function interface and closed loop function. 
	** @brief   : 具有多功能接口和闭环功能的步进电机
	** @author  : unlir (知不知啊)
	** @contacts: QQ.1354077136
	******
	** @address : https://github.com/unlir/XDrive
	******
	************************************************************************
	******
	** {Stepper motor with multi-function interface and closed loop function.}
	** Copyright (c) {2020}  {unlir(知不知啊)}
	** 
	** This program is free software: you can redistribute it and/or modify
	** it under the terms of the GNU General Public License as published by
	** the Free Software Foundation, either version 3 of the License, or
	** (at your option) any later version.
	** 
	** This program is distributed in the hope that it will be useful,
	** but WITHOUT ANY WARRANTY; without even the implied warranty of
	** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	** GNU General Public License for more details.
	** 
	** You should have received a copy of the GNU General Public License
	** along with this program.  If not, see <http://www.gnu.org/licenses/>.
	******
	************************************************************************
******/

/*****
  ** @file     : Location_Tracker.c/h
  ** @brief    : 位置跟踪器
  ** @versions : 1.1.7
  ** @time     : 2019/12/06
  ** @reviser  : unli (JiXi XuanCheng China)
  ** @explain  : null
*****/

//Oneself
#include "position_tracker.hpp"
#include <cstdlib> // for abs
//#include <cmath>   // for fabs, if needed

/****************************************  位置跟踪器  ****************************************/
/****************************************  位置跟踪器  ****************************************/
/****************************************  位置跟踪器  ****************************************/

/**
  * 位置跟踪器设置最大速度
  * @param  value		最大速度
  * @retval true:成功 / false:错误
**/
void PositionTracker::Set_MaxSpeed(int32_t value)
{
	value = abs(value);
	if((value > 0) && (value <= Move_Rated_Speed))
	{
		m_max_speed = value;
		m_valid_max_speed = true;
	}
	else
	{
		m_valid_max_speed = false;
	}
}

/**
  * 位置跟踪器设置加速加速度
  * @param  value		加速加速度	
  * @retval true:成功 / false:错误
**/
void PositionTracker::Set_UpAcc(int32_t value)
{
	value = abs(value);
	if((value > 0) && (value <= Move_Rated_UpAcc))
	{
		m_up_acc = value;
		m_valid_up_acc = true;
	}
	else
	{
		m_valid_up_acc = false;
	}
}

/**
  * 位置跟踪器设置减速加速度
  * @param  tracker		位置跟踪器实例
  * @param  value		减速加速度	
  * @retval true:成功 / false:错误
**/
void PositionTracker::Set_DownAcc(int32_t value)
{
	value = abs(value);
	if((value > 0) && (value <= Move_Rated_UpAcc))
	{
		m_down_acc = value;
		m_down_acc_quick = 0.5f / (float)m_down_acc;
		m_valid_down_acc = true;
	}
	else
	{
		m_valid_down_acc = false;
	}
}

/**
  * 位置跟踪器参数恢复
  * @param  NULL
  * @retval NULL
**/
void PositionTracker::Set_Default(void)
{
	PositionTracker::Set_MaxSpeed(DE_MAX_SPEED);
	PositionTracker::Set_UpAcc(DE_UP_ACC);
	PositionTracker::Set_DownAcc(DE_Down_ACC);
}

/**
  * 位置跟踪器初始化
  * @param  tracker		位置跟踪器实例
  * @retval NULL
**/
void PositionTracker::Init()
{
	//前置配置无效时,加载默认配置
	if(!m_valid_max_speed)	{	PositionTracker::Set_MaxSpeed(DE_MAX_SPEED);		}
	if(!m_valid_up_acc)		{	PositionTracker::Set_UpAcc(DE_UP_ACC);	}
	if(!m_valid_down_acc)	{	PositionTracker::Set_DownAcc(DE_Down_ACC);	}
	
	
	//静态配置的跟踪参数
	m_speed_locking_stop = Move_Pulse_NUM;	//每秒1转时允许拉停	(最佳的值应该为加速度/1000，但是存在两个加速度，不方便取值)
																								//(正确解决方法应该为将减速位移运算转换为整形运算)
	//计算过程参数
	m_course_acc_integral = 0;
	m_course_speed = 0;
	m_course_speed_integral = 0;
	m_course_location = 0;
	//输出跟踪控制量
	m_go_location = 0;
	m_go_speed = 0;
}

/**
  * 位置跟踪器开始新任务
  * @param  tracker			位置跟踪器实例
  * @param  real_location	实时位置
  * @param  real_speed		实时速度
  * @retval NULL
**/
void PositionTracker::NewTask(int32_t real_location, int32_t real_speed)
{
	//更新计算过程数据
	m_course_acc_integral = 0;			//过程加速度积分
	m_course_speed = real_speed;			//过程速度
	m_course_speed_integral = 0;			//过程速度积分
	m_course_location = real_location;	//过程位置
}

/**
  * 位置跟踪器速度积分
  * @param  value	加速度
**/
#define Speed_Course_Integral(value)	\
{	\
	m_course_acc_integral += value;											\
	m_course_speed += m_course_acc_integral / CONTROL_FREQ_HZ;		\
	m_course_acc_integral = m_course_acc_integral % CONTROL_FREQ_HZ;	\
}								//(C语言除法运算向0取整，直接取余即可)

/**
  * 位置跟踪器位置积分
  * @param  value	速度
**/
#define Location_Course_Integral(value)	\
{	\
	m_course_speed_integral += value;											\
	m_course_location += m_course_speed_integral / CONTROL_FREQ_HZ;		\
	m_course_speed_integral = m_course_speed_integral % CONTROL_FREQ_HZ;	\
}								//(C语言除法运算向0取整，直接取余即可)

/**
  * 位置跟踪器获得立即位置和立即速度
  * @param  tracker			位置跟踪器实例
  * @param  goal_location	目标位置
  * @retval NULL
**/
void PositionTracker::Capture_Goal(int32_t goal_location)
{
	//整形位置差
	int32_t location_sub = goal_location - m_course_location;

	/********************到达目标********************/
	if(location_sub == 0)
	{
		/******************** 速度小于刹停速度********************/
		if((m_course_speed >= -m_speed_locking_stop) && (m_course_speed <= m_speed_locking_stop))
		{
			//进入静止->取整浮点数据
			m_course_acc_integral = 0;
			m_course_speed = 0;
			m_course_speed_integral = 0;
		}
		/********************速度 > 0********************/
		else if(m_course_speed > 0)
		{
			//正向减速到0
			Speed_Course_Integral(-m_down_acc)
			if(m_course_speed <= 0)
			{
				m_course_acc_integral = 0;
				m_course_speed = 0;
			}
		}
		/********************速度 < 0********************/
		else if(m_course_speed < 0)
		{
			//反向减速到0
			Speed_Course_Integral(m_down_acc)
			if(m_course_speed >= 0)
			{
				m_course_acc_integral = 0;
				m_course_speed = 0;
			}
		}
	}
	/********************未到达目标********************/
	else
	{
		/********************速度为0********************/
		if(m_course_speed == 0)
		{
			if(location_sub > 0)
			{
				Speed_Course_Integral(m_up_acc)	//开始正向加速
			}
			else
			{
				Speed_Course_Integral(-m_up_acc)	//开始反向加速
			}
		}
		/********************速度与位移方向同向(正方向)********************/
		else if((location_sub > 0) && (m_course_speed > 0))
		{
			if(m_course_speed <= m_max_speed)
			{
				//计算需要的减速位移
				int32_t need_down_location = (int32_t)((float)m_course_speed * (float)m_course_speed * (float)m_down_acc_quick);	//浮点运算
				if(abs(location_sub) > need_down_location)
				{
					//正向加速到最大速度
					if(m_course_speed < m_max_speed)
					{
						Speed_Course_Integral(m_up_acc);
						if(m_course_speed >= m_max_speed)
						{
							m_course_acc_integral = 0;
							m_course_speed = m_max_speed;
						}
					}
					//正向稳速
					//else if(m_course_speed == m_max_speed_max)
					//{}
					//正向超速->减速
					else if(m_course_speed > m_max_speed)
					{
						Speed_Course_Integral(-m_down_acc);
					}
				}
				else
				{
					//正向减速到0
					Speed_Course_Integral(-m_down_acc);
					if(m_course_speed <= 0)
					{
						m_course_acc_integral = 0;
						m_course_speed = 0;
					}
				}
			}
			else
			{
				//正向减速到0
				Speed_Course_Integral(-m_down_acc);
				if(m_course_speed <= 0)
				{
					m_course_acc_integral = 0;
					m_course_speed = 0;
				}
			}
		}
		/********************速度与位移方向同向(负方向)********************/
		else if((location_sub < 0) && (m_course_speed < 0))
		{
			if(m_course_speed >= -m_max_speed)
			{
				//计算需要的减速位移(有溢出风险的运算)
				int32_t need_down_location = (int32_t)((float)m_course_speed * (float)m_course_speed * (float)m_down_acc_quick);	//浮点运算
				if(abs(location_sub) > need_down_location)
				{
					//反向加速到最大速度
					if(m_course_speed > -m_max_speed)
					{
						Speed_Course_Integral(-m_up_acc);
						if(m_course_speed <= -m_max_speed)
						{
							m_course_acc_integral = 0;
							m_course_speed = -m_max_speed;
						}
					}
					//反向稳速
					//else if(m_course_speed == -m_max_speed_max)
					//{}
					//反向超速->减速
					else if(m_course_speed < -m_max_speed)
					{
						Speed_Course_Integral(m_down_acc);
					}
				}
				else
				{
					//反向减速到0
					Speed_Course_Integral(m_down_acc);
					if(m_course_speed >= 0)
					{
						m_course_acc_integral = 0;
						m_course_speed = 0;
					}
				}
			}
			else
			{
				//反向减速到0
				Speed_Course_Integral(m_down_acc);
				if(m_course_speed >= 0)
				{
					m_course_acc_integral = 0;
					m_course_speed = 0;
				}
			}
		}
		/********************速度与位移方向反向********************/
		else if((location_sub < 0) && (m_course_speed > 0))
		{
			//正向减速到0
			Speed_Course_Integral(-m_down_acc);
			if(m_course_speed <= 0)
			{
				m_course_acc_integral = 0;
				m_course_speed = 0;
			}
		}
		else if(((location_sub > 0) && (m_course_speed < 0)))
		{
			//反向减速到0
			Speed_Course_Integral(m_down_acc);
			if(m_course_speed >= 0)
			{
				m_course_acc_integral = 0;
				m_course_speed = 0;
			}
		}		
	}

	//位置积分
	Location_Course_Integral(m_course_speed);

	//输出
	m_go_location = (int32_t)m_course_location;
	m_go_speed = (int32_t)m_course_speed;
}