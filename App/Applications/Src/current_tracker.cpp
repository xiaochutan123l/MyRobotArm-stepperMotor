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
  ** @file     : Current_Tracker.c/h
  ** @brief    : 电流跟踪器
  ** @versions : 1.1.7
  ** @time     : 2019/12/06
  ** @reviser  : unli (JiXi XuanCheng China)
  ** @explain  : null
*****/

#include "current_tracker.hpp"
#include <cstdlib> // for abs

/****************************************  电流跟踪器  ****************************************/
/****************************************  电流跟踪器  ****************************************/
/****************************************  电流跟踪器  ****************************************/

/**
  * 电流跟踪器设置增流变化率
  * @param  value:	增流梯度
  * @retval NULL
**/
void CurrentTracker::Set_UpRate(int32_t value)
{
	value = abs(value);
	if((value > 0) && (value <= Move_Rated_UpCurrentRate))
	{
		m_up_rate = value;
		m_valid_up_rate = true;
	}
	else{
		m_valid_up_rate = false;
	}
}

/**
  * 电流跟踪器设置减流变化率
  * @param  value:	减流梯度
  * @retval NULL
**/
void CurrentTracker::Set_DownRate(int32_t value)
{
	value = abs(value);
	if((value > 0) && (value <= Move_Rated_DownCurrentRate))
	{
		m_down_rate = value;
		m_valid_down_rate = true;
	}
	else{
		m_valid_down_rate = false;
	}
}

/**
  * 电流跟踪器参数恢复
  * @param  NULL
  * @retval NULL
**/
void CurrentTracker::Set_Default(void)
{
	Set_UpRate(De_Up_Rate);
	Set_DownRate(De_Down_Rate);
}

/**
  * 电流跟踪器初始化
  * @param  NULL
  * @retval NULL
**/
void CurrentTracker::Init(void)
{
	//前置配置无效时,加载默认配置
	if(!m_valid_up_rate)			{	Set_UpRate(De_Up_Rate);			}
	if(!m_valid_down_rate)		{	Set_DownRate(De_Down_Rate);	}

	//计算过程数据
	m_course_mut = 0;
	m_course = 0;
	//输出跟踪量
	m_go_current = 0;
}

/**
  * 电流跟踪器初始化
  * @param  real_current	实时电流
  * @retval NULL
**/
void CurrentTracker::NewTask(int16_t real_current)
{
	//更新计算过程数据
	m_course_mut = 0;				//过程电流
	m_course = real_current;	//过程电流
}

/**
  * 电流跟踪器电流积分
  * @param  value	梯度
**/
#define Current_Course_Integral(value)	\
{	\
	m_course_mut += value;											\
	m_course += m_course_mut / CONTROL_FREQ_HZ;		\
	m_course_mut = m_course_mut % CONTROL_FREQ_HZ;	\
}

/**
  * 电流跟踪器获得立即电流
  * @param  tracker			电流跟踪器实例
  * @param  goal_current	目标电流
  * @retval NULL
**/
void CurrentTracker::Capture_Goal(int32_t goal_current)
{
	//整形电流差
	int32_t electric_sub = goal_current - m_course;	//电流差	= 控制器目标电流 - 跟踪器电流

	/********************电流到达目标电流********************/
	if(electric_sub == 0)
	{
		m_course = goal_current;		//进入稳流状态
	}
	/********************矢量电流小于目标********************/
	else if(electric_sub > 0)
	{
		if(m_course >= 0)
		{
			Current_Course_Integral(m_up_rate);
			if(m_course >= goal_current)
			{
				m_course_mut = 0;
				m_course = goal_current;
			}
		}
		else// if(m_course < 0)
		{
			Current_Course_Integral(m_down_rate);
			if((int32_t)m_course >= 0)
			{
				m_course_mut = 0;
				m_course = 0;
			}
		}
	}
	/********************矢量电流大于目标********************/
	else if(electric_sub < 0)
	{
		if(m_course <= 0)
		{
			Current_Course_Integral(-m_up_rate);
			if((int32_t)m_course <= (int32_t)goal_current)
			{
				m_course_mut = 0;
				m_course = goal_current;
			}
		}
		else// if(m_course > 0)
		{
			Current_Course_Integral(-m_down_rate);
			if((int32_t)m_course <= 0)
			{
				m_course_mut = 0;
				m_course = 0;
			}
		}
	}

	//输出
	m_go_current = (int32_t)m_course;
}
