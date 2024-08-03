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
  ** @file     : Move_Reconstruct.c/h
  ** @brief    : 运动规划器
  ** @versions : 1.1.7
  ** @time     : 2019/12/06
  ** @reviser  : unli (JiXi XuanCheng China)
  ** @explain  : null
*****/

//Oneself
#include "move_reconstructor.hpp"
#include <cstdlib> // for abs

//Control.h


/****************************************  运动重构器  ****************************************/
/****************************************  运动重构器  ****************************************/
/****************************************  运动重构器  ****************************************/

/**
  * 运动重构器设置减速加速度
  * @param  reconstruct		运动重构器实例
  * @retval true:成功 / false:错误
**/
void MoveReconstructor::Set_DownAcc(int32_t value)
{
	value = abs(value);
	if((value > 0) && (value <= Move_Rated_DownAcc))
	{
		m_down_acc = value;
		m_valid_down_acc = true;
	}
	else{
		m_valid_down_acc = false;
	}
}

/**
  * 运动重构器设置超时时间
  * @param  reconstruct		运动重构器实例
  * @retval true:成功 / false:错误
**/
void MoveReconstructor::Set_OverTime(uint16_t value)
{
	if((value >= Reconstruct_Min_OverTime) && (value <= Reconstruct_Max_OverTime))
	{
		m_overtime = value;
		m_valid_overtime = true;
	}
	else{
		m_valid_overtime = false;
	}
}

/**
  * 运动重构器参数恢复
  * @param  NULL
  * @retval NULL
**/
void MoveReconstructor::Set_Default(void)
{
	Set_DownAcc(DE_DOWN_ACC);
	Set_OverTime(DE_OverTime);	
}

/**
  * 运动重构器初始化
  * @param  reconstruct		运动重构器实例
  * @retval NULL
**/
void MoveReconstructor::Init(void)
{
	//前置配置无效时,加载默认配置
	if(!m_valid_down_acc)	{	Set_DownAcc(DE_DOWN_ACC);	}
	if(!m_valid_overtime)	{	Set_OverTime(DE_OverTime);	}
	
	//动态跟踪参数
	m_dyn_speed_acc = 0;
	//源信号记录
	m_record_timer = 0;
	m_overtime_flag = false;
	m_record_speed = 0;
	m_record_location = 0;
	//计算过程数据
	m_speed_course_dec = 0;
	m_speed_course = 0;
	m_location_course_dec = 0;
	m_location_course = 0;
	//输出跟踪控制量
	m_go_location = 0;
	m_go_speed = 0;
}

/**
  * 运动重构器开始新任务
  * @param  reconstruct		运动重构器实例
  * @param  real_location	实时位置
  * @param  real_speed		实时速度
  * @retval NULL
**/
void MoveReconstructor::NewTask(int32_t real_location, int32_t real_speed)
{
	//源信号记录
	m_record_timer = 0;			//信号源计时器清零
	m_overtime_flag = false;		//超时标志清位
	//更新计算过程数据
	m_speed_course_dec = 0;				//过程速度
	m_speed_course = real_speed;			//过程速度
	m_location_course_dec = 0;			//过程位置
	m_location_course = real_location;	//过程位置
}

/**
  * 运动重构器速度积分
  * @param  value	加速度
**/
#define Speed_Course_Integral(value)	\
{	\
	m_speed_course_dec += value;												\
	m_speed_course += m_speed_course_dec / CONTROL_FREQ_HZ;		\
	m_speed_course_dec = m_speed_course_dec % CONTROL_FREQ_HZ;	\
}								//(C语言除法运算向0取整，直接取余即可)

/**
  * 运动重构器位置积分
  * @param  value	速度
**/
#define Location_Course_Integral(value)	\
{	\
	m_location_course_dec += value;												\
	m_location_course += m_location_course_dec / CONTROL_FREQ_HZ;		\
	m_location_course_dec = m_location_course_dec % CONTROL_FREQ_HZ;	\
}								//(C语言除法运算向0取整，直接取余即可)

/**
  * 运动重构器获得立即位置和立即速度
  * @param  reconstruct		运动重构器实例
  * @param  goal_location	目标位置
  * @param  goal_speed		目标速度
  * @retval NULL
**/
void MoveReconstructor::Capture_Goal(int32_t goal_location, int32_t goal_speed)
{
	//触发新目标
	if( (goal_speed != m_record_speed)
	 || (goal_location != m_record_location))
	{
		//源信号记录
		m_record_timer = 0;
		m_record_speed = goal_speed;
		m_record_location = goal_location;
		//更新动态跟踪参数
		m_dyn_speed_acc = (int32_t)(   (float)(goal_speed + m_speed_course) * (float)(goal_speed - m_speed_course)
											    / (float)(2 * (goal_location - m_location_course))
											  );
		//过程数据
		m_overtime_flag = false;
	}
	else
	{
		//源信号超时处理
		if(m_record_timer >= (200 * 1000))
			m_overtime_flag = true;
		else
			m_record_timer += CONTROL_PERIOD_US;
	}

	//获得运动更改
	if(m_overtime_flag)
	{//超时->开始减速为0
		if(m_speed_course == 0)
		{
			m_speed_course_dec = 0;
			m_speed_course = 0;
		}
		else if(m_speed_course > 0)
		{
			Speed_Course_Integral(-m_down_acc);
			if(m_speed_course <= 0)
			{
				m_speed_course_dec = 0;
				m_speed_course = 0;
			}
		}
		else
		{
			Speed_Course_Integral(m_down_acc);
			if(m_speed_course >= 0)
			{
				m_speed_course_dec = 0;
				m_speed_course = 0;
			}
		}
	}
	else
	{//未超时->按照预设加速度变速
		Speed_Course_Integral(m_dyn_speed_acc);
	}

	//位置积分
	Location_Course_Integral(m_speed_course);

	//输出
	m_go_location = (int32_t)m_location_course;
	m_go_speed = (int32_t)m_speed_course;
}


