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

#ifndef HPP
#define HPP

#ifdef __cplusplus
extern "C" {
#endif

#include "control_config.h"

/****************************************  速度跟踪器  ****************************************/
/****************************************  速度跟踪器  ****************************************/
/****************************************  速度跟踪器  ****************************************/
/**
  * Speed_Tracker类结构体定义
**/
#define	DE_UP_ACC			(Move_Rated_UpAcc / 10)
#define	DE_DOWN_ACC		    (Move_Rated_DownAcc / 10)

class SpeedTracker {
public:
	//速度跟踪器设置加速加速度
	void Set_UpAcc(int32_t value);		
	//速度跟踪器设置减速加速度
	void Set_DownAcc(int32_t value);
	//速度跟踪器参数恢复	
	void Set_Default(void);						
	//速度跟踪器初始化
	void Init(void);		
	//速度控制器开始新任务										
	void NewTask(int32_t real_speed);		
	//速度跟踪器获得立即速度		
	void Capture_Goal(int32_t goal_speed);	

	int32_t getGoSpeed() {
		return m_go_speed;
	};

private:
	//配置(加速加速度)
	bool m_valid_up_acc;
	int32_t	m_up_acc;
	//配置(减速加速度)
	bool m_valid_down_acc;
	int32_t	m_down_acc;
	//计算过程数据
	//过程加速度积分(放大CONTROL_FREQ_HZ倍)
	int32_t	m_course_mut;	
	//过程速度
	int32_t	m_course;			
	//输出跟踪量
	//立即速度
	int32_t	m_go_speed;		
};

#ifdef __cplusplus
}
#endif

#endif //HPP