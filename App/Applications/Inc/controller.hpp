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
  ** @file     : control_config.c/h
  ** @brief    : 电机控制
  ** @versions : 2.2.3
  ** @time     : 2020/09/15
  ** @reviser  : unli (HeFei China)
  ** @explain  : null
*****/

#ifndef CONTROLLER_HPP
#define CONTROLLER_HPP

#include "control_config.h"
#include "pid.hpp"
#include "dce.hpp"
#include "motor.hpp"
#include "encoder.hpp"
#include "speed_tracker.hpp"
#include "position_tracker.hpp"
#include "current_tracker.hpp"
#include "move_reconstructor.hpp"

/****************************************  Motor_Control  ****************************************/
/****************************************  Motor_Control  ****************************************/
/**
  * 控制器状态
**/
typedef enum{
	Control_State_Stop				= 0x00,	//停止
	Control_State_Finish			= 0x01,	//任务完成
	Control_State_Running			= 0x02,	//任务执行中
	Control_State_Overload		= 0x03,	//过载
	Control_State_Stall				= 0x04,	//堵转
}Motor_State;

#define		De_Motor_Mode		Motor_Mode_Digital_Location	//默认配置

/**
  * 模式
**/
typedef enum{
	/*****************************************************外部输入量***********信号发生器(可控的运动参数)**开放状态(开放版本)*****/
	//停止
	Control_Mode_Stop						= 0x10,	//停止						NULL									NULL												NULL
	//DIG(CAN/RS485)
	Motor_Mode_Digital_Location	= 0x20,	//DIG位置					目标位置							位置跟踪器(速度,双加速度)		源码开放(20191101)
	Motor_Mode_Digital_Speed		= 0x21,	//DIG速度					目标速度							速度跟踪器(双加速度)				源码开放(20191101)
	Motor_Mode_Digital_Current	= 0x22,	//DIG电流					目标电流							电流跟踪器(双电梯度)				源码开放(20191101)
	Motor_Mode_Digital_Track		= 0x23,	//DIG轨迹					多指令								运动重构器(运动自寻)				源码开放(20191101)
}Motor_Mode;

class Controller {
public:
	Controller(Motor *motor, Encoder *encoder) : m_motor(motor), m_encoder(encoder) {};
	//参数配置
	void SetMotorMode(Motor_Mode _mode);	//控制模式
	void SetDefault(void);								//控制模式参数恢复

	//数据写入
	void Write_Goal_Location(int32_t value);//写入目标位置
	void Write_Goal_Speed(int32_t value);		//写入目标速度
	void Write_Goal_Current(int16_t value);	//写入目标电流
	void Write_Goal_Disable(uint16_t value);//写入目标失能
	void Write_Goal_Brake(uint16_t value);	//写入目标刹车

	//任务执行
	void Init(void);											//电机控制初始化
	void Callback(void);									//控制器任务回调
	void Clear_Integral(void);						//清除积分
	void Clear_Stall(void);								//清除堵转保护
	int32_t AdvanceCompen(int32_t _speed);//超前角补偿

	void Control_Cur_To_Electric(int16_t current);
	void Control_DCE_To_Electric(int32_t _location, int32_t _speed);
	void Control_PID_To_Electric(int32_t _speed);

//private:
	Pid m_pid;
	Dce m_dce;
	Motor *m_motor;
	Encoder *m_encoder;
	SpeedTracker m_speed_tracker;
	PositionTracker m_position_tracker;
	CurrentTracker m_current_tracker;
	MoveReconstructor m_move_reconstructor;

	bool		m_valid_mode;		//有效标志
	Motor_Mode	m_mode_order;		//电机模式_新指令的

	//模式
	Motor_Mode	m_mode_run;			//电机模式_运行中的
	//读取
	int32_t		m_real_lap_location;				//读取单圈位置
	int32_t		m_real_lap_location_last;		//读取单圈位置
	int32_t		m_real_location;						//读取位置
	int32_t		m_real_location_last;				//读取位置
	//估计
	int32_t		m_est_speed_mut;						//估计速度倍值(放大n倍)(mut有积分作用,不会有精度损失)
	int32_t		m_est_speed;								//估计速度
	int32_t		m_est_lead_location;				//估计位置超前位置
	int32_t		m_est_lead_location_debug;	//估计位置超前位置
	int32_t		m_est_location;							//估计位置
	int32_t		m_est_error;								//估计位置误差
	//硬目标
	int32_t		m_goal_location;	//目标位置(由信号输入)
	int32_t		m_goal_speed;			//目标速度(由信号输入)
	int16_t		m_goal_current;		//目标电流(由信号输入)

	//软目标
	int32_t		m_soft_location;	//软位置(由 跟踪器/重构器/插补器/硬运算 得到)
	int32_t		m_soft_speed;			//软速度(由 跟踪器/重构器/插补器/硬运算 得到)
	int16_t		m_soft_current;		//软电流(由 跟踪器/重构器/插补器/硬运算 得到)
	bool		m_soft_new_curve;	//新软目标曲线
	//输出
	int32_t		m_foc_location;		//FOC矢量位置
	int32_t		m_foc_current;		//FOC矢量大小
	//状态
	Motor_State	m_state;			//统一的电机状态
};

#endif // CONTROLLER_HPP