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
  ** @file     : Speed_Tracker.c/h
  ** @brief    : 速度跟踪器
  ** @versions : 1.1.7
  ** @time     : 2019/12/06
  ** @reviser  : unli (JiXi XuanCheng China)
  ** @explain  : null
*****/

//Oneself
#include "speed_Tracker.hpp"
#include <cstdlib> // for abs
/****************************************  速度跟踪器  ****************************************/
/****************************************  速度跟踪器  ****************************************/
/****************************************  速度跟踪器  ****************************************/

/**
  * 速度跟踪器设置加速加速度
  * @param  tracker		速度跟踪器实例
  * @retval true:成功 / false:错误
**/
void SpeedTracker::Set_UpAcc(int32_t value)
{
	value = abs(value);
	if((value > 0) && (value <= Move_Rated_UpAcc))
	{
		m_up_acc = value;
		m_valid_up_acc = true;
	}
	else{
		m_valid_up_acc = false;
	}
}

/**
  * 速度跟踪器设置减速加速度
  * @param  tracker		速度跟踪器实例
  * @retval true:成功 / false:错误
**/
void SpeedTracker::Set_DownAcc(int32_t value)
{
	value = abs(value);
	if((value > 0) && (value <= Move_Rated_DownAcc))
	{
		m_down_acc = value;
		m_valid_down_acc = true;
	}
	else{
		m_valid_down_acc = true;
	}
}

/**
  * 速度跟踪器参数恢复
  * @param  NULL
  * @retval NULL
**/
void SpeedTracker::Set_Default(void)
{
	SpeedTracker::Set_UpAcc(DE_UP_ACC);
	SpeedTracker::Set_DownAcc(DE_DOWN_ACC);
}

/**
  * 速度跟踪器初始化
  * @param  tracker		速度跟踪器实例
  * @retval NULL
**/
void SpeedTracker::Init(void)
{
	//前置配置无效时,加载默认配置
	if(!m_valid_up_acc)			{	SpeedTracker::Set_UpAcc(DE_UP_ACC);			}
	if(!m_valid_down_acc)		{	SpeedTracker::Set_DownAcc(DE_DOWN_ACC);	}
	
	//计算过程数据
	m_course_mut = 0;
	m_course = 0;
	//输出跟踪量
	m_go_speed = 0;
}

/**
  * 速度跟踪器开始新任务
  * @param  tracker			速度跟踪器实例
  * @param  real_speed		实时速度
  * @retval NULL
**/
void SpeedTracker::NewTask(int32_t real_speed)
{
	//更新计算过程数据
	m_course_mut = 0;		//过程加速度积分
	m_course = real_speed;		//过程速度
}

/**
  * 速度跟踪器速度积分
  * @param  value	加速度
**/
#define Speed_Course_Integral(value)	\
{	\
	m_course_mut += value;											\
	m_course += m_course_mut / CONTROL_FREQ_HZ;		\
	m_course_mut = m_course_mut % CONTROL_FREQ_HZ;	\
}

/**
  * 速度跟踪器获得立即速度
  * @param  tracker			速度跟踪器实例
  * @param  goal_speed		目标速度
  * @retval NULL
**/
void SpeedTracker::Capture_Goal(int32_t goal_speed)
{
	//整形速度差
	int32_t speed_sub = goal_speed - m_course;

	/********************速度到达目标速度********************/
	if(speed_sub == 0)
	{
		m_course = goal_speed;		//进入稳速状态
	}
	/********************矢量速度小于目标********************/
	else if(speed_sub > 0)
	{
		if(m_course >= 0)
		{
			Speed_Course_Integral(m_up_acc);
			if(m_course >= goal_speed)
			{
				m_course_mut = 0;
				m_course = goal_speed;
			}
		}
		else
		{
			Speed_Course_Integral(m_down_acc);
			if(m_course >= 0)
			{
				m_course_mut = 0;
				m_course = 0;
			}
		}
	}
	/********************矢量速度大于目标********************/
	else if(speed_sub < 0)
	{
		if(m_course <= 0)
		{
			Speed_Course_Integral(-m_up_acc);
			if(m_course <= goal_speed)
			{
				m_course_mut = 0;
				m_course = goal_speed;
			}
		}
		else
		{
			Speed_Course_Integral(-m_down_acc);
			if(m_course <= 0)
			{
				m_course_mut = 0;
				m_course = 0;
			}
		}
	}

	//输出
	m_go_speed = (int32_t)m_course;
}