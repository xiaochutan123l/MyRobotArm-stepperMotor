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

//Oneself
#include "controller.hpp"

/**
  * @brief  电流输出
  * @param  current: 输出电流
  * @retval NULL
**/
void Controller::Control_Cur_To_Electric(int16_t current)
{
	//输出FOC电流
	m_foc_current = current;
	//输出FOC位置
	if(m_foc_current > 0)		m_foc_location = m_est_location + Move_Divide_NUM;
	else if(m_foc_current < 0)	m_foc_location = m_est_location - Move_Divide_NUM;
	else						m_foc_location = m_est_location;
	//输出任务到驱动
	m_motor->SetFocCurrentVector(m_foc_location, m_foc_current);
	//CurrentControl_Out_FeedTrack(m_foc_location, m_foc_current, false, true);
}

/**
  * @brief  PID电流控制
  * @param  _speed    控制速度
  * @retval NULL
**/
void Controller::Control_PID_To_Electric(int32_t _speed)
{
	//输出FOC电流
	m_foc_current = m_pid.CalcOutput(_speed, m_est_speed);
	//输出FOC位置
	if(m_foc_current > 0)		m_foc_location = m_est_location + Move_Divide_NUM;
	else if(m_foc_current < 0)	m_foc_location = m_est_location - Move_Divide_NUM;
	else						m_foc_location = m_est_location;
	//输出任务到驱动
	m_motor->SetFocCurrentVector(m_foc_location, m_foc_current);
	//CurrentControl_Out_FeedTrack(m_foc_location, m_foc_current, false, true);
}

/**
  * @brief  DCE电流控制
  * @param  _location 控制位置
  * @param  _speed    控制速度
  * @retval NULL
**/
void Controller::Control_DCE_To_Electric(int32_t _location, int32_t _speed)
{
	//输出FOC电流
	m_foc_current = m_dce.CalcOutput(_location, _speed, m_est_location, m_est_speed);
	//输出FOC位置
	if(m_foc_current > 0)		m_foc_location = m_est_location + Move_Divide_NUM;
	else if(m_foc_current < 0)	m_foc_location = m_est_location - Move_Divide_NUM;
	else						m_foc_location = m_est_location;
	//输出任务到驱动
	m_motor->SetFocCurrentVector(m_foc_location, m_foc_current);
	//CurrentControl_Out_FeedTrack(m_foc_location, m_foc_current, false, true);
}

/****************************************  m_  ****************************************/
/****************************************  m_  ****************************************/

/**
  * @brief  电机模式配置
  * @param  _mode
  * @retval NULL
**/
void Controller::SetMotorMode(Motor_Mode _mode)
{
	m_mode_order = _mode;
	m_valid_mode = true;
}

/**
  * @brief  控制模式参数恢复
  * @param  NULL
  * @retval NULL
**/
void Controller::SetDefault(void)
{
	SetMotorMode(De_Motor_Mode);
}

/**
  * @brief  堵转保护开关
  * @param  _switch
  * @retval NULL
**/
void Controller::SetStallSwitch(bool _switch)
{
	m_stall_switch = _switch;
	m_valid_stall_switch = true;
}

/**
  * @brief  写入目标位置
  * @param  NULL
  * @retval NULL
**/
void Controller::Write_Goal_Location(int32_t value)
{
	m_goal_location = value;
}
	
/**
  * @brief  写入目标速度
  * @param  NULL
  * @retval NULL
**/
void Controller::Write_Goal_Speed(int32_t value)
{
	if((value >= -Move_Rated_Speed) && (value <= Move_Rated_Speed))
	{
		m_goal_speed = value;
	}
}

/**
  * @brief  写入目标电流
  * @param  NULL
  * @retval NULL
**/
void Controller::Write_Goal_Current(int16_t value)
{
	if((value >= -Current_Rated_Current) && (value <= Current_Rated_Current))
	{
		m_goal_current = value;
	}
}

/**
  * @brief  电机控制初始化
  * @param  NULL
  * @retval NULL
**/
void Controller::Init(void)
{	
	//前置配置无效时,加载默认配置
	if(!m_valid_mode) {		
		SetMotorMode(De_Motor_Mode);		
	}

	//模式
	m_mode_run = Motor_Mode_Digital_Location;
	//读取
	m_real_lap_location = 0;
	m_real_lap_location_last = 0;
	m_real_location = 0;
	m_real_location_last = 0;
	//估计
	m_est_speed_mut = 0;
	m_est_speed = 0;
	m_est_lead_location = 0;
	m_est_location = 0;
	m_est_error = 0;
	//硬目标
	m_goal_location = 0;
	m_goal_speed = 0;
	m_goal_current = 0;
	m_goal_disable = false;
	m_goal_brake = false;
	//软目标
	m_soft_location = 0;
	m_soft_speed = 0;
	m_soft_current = 0;
	m_soft_new_curve = false;
	m_soft_disable = false;
	m_soft_brake = false;
	//输出
	m_foc_location = 0;
	m_foc_current = 0;
	//堵转识别
	m_stall_time_us = 0;
	m_stall_flag = false;
	//过载识别
	m_overload_time_us = 0;
	m_overload_flag = false;
	
	//状态
	m_state = Control_State_Stop;		
	
	/**********  控制算法初始化  **********/
	m_pid.Init();
	m_dce.Init();
	
	/********** 轨迹规划 **********/
	m_position_tracker.Init();
	m_speed_tracker.Init();
	m_current_tracker.Init();
	m_move_reconstructor.Init();

	/********** 传感器初始化 **********/
	if (!m_motor->isInit()) {
		m_motor->init();
	}
	if (!m_encoder->isInit()) {
		m_encoder->init();
	}
}

/**
  * @brief  控制器任务回调
  * @param  NULL
  * @retval NULL
**/
void Controller::Callback(void)
{
	/************************************ 首次进入控制回调 ************************************/
	/************************************ 首次进入控制回调 ************************************/
	static bool first_call = true;
	if(first_call)
	{
		uint16_t rect_angle = m_encoder->updateRectAngle();
		//读取(为了方便将XDrive代码移植到软件编码器,将位置读取初始化部分全部放置在此处运行)
		m_real_lap_location				= rect_angle;
		m_real_lap_location_last	= rect_angle;
		m_real_location						= rect_angle;
		m_real_location_last			= rect_angle;
		//第一次运行强制退出
		first_call = false;
		// TODO: remove
		//Write_Goal_Location(1536800);
		//Write_Goal_Location(8800); 
		//SetMotorMode(Motor_Mode_Digital_Speed);
		//Write_Goal_Speed(500000);
		return;
	}
	
	/************************************ 数据采集 ************************************/
	/************************************ 数据采集 ************************************/
	int32_t		sub_data;		//用于各个算差
	//读取单圈位置
	m_real_lap_location_last = m_real_lap_location;
	m_real_lap_location = m_encoder->updateRectAngle();
	//回环检测
	sub_data = m_real_lap_location - m_real_lap_location_last;
	if(sub_data > (Move_Pulse_NUM >> 1))				sub_data -= Move_Pulse_NUM;
	else if(sub_data < -(Move_Pulse_NUM >> 1))	sub_data += Move_Pulse_NUM;
	//读取位置
	m_real_location_last = m_real_location;
	m_real_location += sub_data;
	
	/************************************ 数据估计 ************************************/
	/************************************ 数据估计 ************************************/
	//估计速度
	m_est_speed_mut += (	((m_real_location - m_real_location_last) * (CONTROL_FREQ_HZ))
																	+ ((int32_t)(m_est_speed  << 5) - (int32_t)(m_est_speed))
																	);
	m_est_speed      = (m_est_speed_mut >> 5);																	//(取整)(向0取整)(保留符号位)
	m_est_speed_mut  = ((m_est_speed_mut) - ((m_est_speed << 5)));	//(取余)(向0取整)(保留符号位)
	//估计位置
	m_est_lead_location = AdvanceCompen(m_est_speed);
	m_est_location = m_real_location + m_est_lead_location;
	//估计误差
	m_est_error = m_soft_location - m_est_location;
	
	/************************************ 运动控制 ************************************/
	/************************************ 运动控制 ************************************/
	if((m_stall_flag)			//堵转标志置位
	|| (m_soft_disable)		//软目标_失能指令
	|| ((!m_encoder->isRectValid()))			//编码器校准表无效
	){
		Clear_Integral();		//清除积分
		m_foc_location = 0;		//清FOC位置
		m_foc_current = 0;		//清FOC电流
		m_motor->setSleep();	//驱动休眠
		//CurrentControl_OutSleep();			//XDrive采用硬件逻辑电流控制,自动休眠
	}
	//输出刹车
	else if(
		 (m_soft_brake)			//软目标_刹车指令
	){
		Clear_Integral();		//清除积分
		m_foc_location = 0;		//清FOC位置
		m_foc_current = 0;		//清FOC电流
		m_motor->setSleep();	//驱动刹车
		//CurrentControl_OutBrake();			//XDrive采用硬件逻辑电流控制,自动刹车
	}
	else{
		//运行模式分支
		switch(m_mode_run)
		{
			//DIG(CAN/RS485)
			case Control_Mode_Stop:	
				m_motor->setSleep();				
				break;
			case Motor_Mode_Digital_Location:	
				Control_DCE_To_Electric(m_soft_location, m_soft_speed);				
				break;
			case Motor_Mode_Digital_Speed:		
				Control_PID_To_Electric(m_soft_speed);																		
				break;
			case Motor_Mode_Digital_Current:		
				Control_Cur_To_Electric(m_soft_current);																		
				break;
			case Motor_Mode_Digital_Track:		
				Control_DCE_To_Electric(m_soft_location, m_soft_speed);																		
				break;
			//其他非法模式
			default:	
				break;
		}
	}
	

	/************************************ 模式变更 ************************************/
	/************************************ 模式变更 ************************************/
	//变更
	if(m_mode_run != m_mode_order)
	{

		m_mode_run = m_mode_order;
		m_soft_new_curve = true;	
		// switch(m_mode_run){
		// 	//DIG(CAN/RS485)
		// 	case Motor_Mode_Digital_Location:	
		// 		m_soft_new_curve = true;	
		// 		break;
		// 	case Motor_Mode_Digital_Speed:		
		// 		m_soft_new_curve = true;	
		// 		break;
		// 	case Motor_Mode_Digital_Current:		
		// 		m_soft_new_curve = true;	
		// 		break;
		// 	case Motor_Mode_Digital_Track:		
		// 		m_soft_new_curve = true;	
		// 		break;
		// 	//其他非法模式
		// 	default:	
		// 		break;
		// }
	}

	//限制
	if(m_goal_speed	> Move_Rated_Speed)					m_goal_speed	=  Move_Rated_Speed;
	else if(m_goal_speed < -Move_Rated_Speed)			m_goal_speed	= -Move_Rated_Speed;
	if(m_goal_current > Current_Rated_Current)			m_goal_current	=  Current_Rated_Current;
	else if(m_goal_current < -Current_Rated_Current)	m_goal_current	= -Current_Rated_Current;

	//额外的触发新发生器
	if(	 ((m_soft_disable) && (!m_goal_disable))	//失能指令关闭
		|| ((m_soft_brake)   && (!m_goal_brake))		//刹车指令关闭
	){
		m_soft_new_curve = true;
	}

	if(m_soft_new_curve){
		m_soft_new_curve = false;
		//控制重载和功率模块唤醒
		Clear_Integral();	//清除控制积分项目
		Clear_Stall();		//清除堵转识别
		switch(m_mode_run){
			//DIG(CAN/RS485)
			case Control_Mode_Stop:
				break;
			case Motor_Mode_Digital_Location:
				m_position_tracker.NewTask(m_est_location, m_est_speed);	
				break;
			case Motor_Mode_Digital_Speed:
				m_speed_tracker.NewTask(m_est_speed);															
				break;
			case Motor_Mode_Digital_Current:
				m_current_tracker.NewTask(m_foc_current);															
				break;
			case Motor_Mode_Digital_Track:
				m_move_reconstructor.NewTask(m_est_location, m_est_speed);															
				break;
			//其他非法模式
			default:	
				break;
		}
	}

	/************************************ 软目标提取 ************************************/
	/************************************ 软目标提取 ************************************/
	//提取(软位置,软速度,软电流)
	switch(m_mode_run){
		//DIG(CAN/RS485)
		case Control_Mode_Stop:
			break;
		case Motor_Mode_Digital_Location:	
			m_position_tracker.Capture_Goal(m_goal_location);
			m_soft_location = m_position_tracker.getGoPosition();
			m_soft_speed    = m_position_tracker.getGoSpeed();
			break;
		case Motor_Mode_Digital_Speed:
			m_speed_tracker.Capture_Goal(m_goal_speed);
			m_soft_speed    = m_speed_tracker.getGoSpeed();
			break;
		case Motor_Mode_Digital_Current:
			m_current_tracker.Capture_Goal(m_goal_current);
			m_soft_current    = m_current_tracker.getGoCurrent();
			break;
		case Motor_Mode_Digital_Track:
			m_move_reconstructor.Capture_Goal(m_goal_location, m_goal_speed);
			m_soft_location = m_move_reconstructor.getGoPosition();
			m_soft_speed    = m_move_reconstructor.getGoSpeed();
			break;
		//其他非法模式
		default:	
			break;
	}
	//提取(软失能,软刹车)
	m_soft_disable = m_goal_disable;
	m_soft_brake = m_goal_brake;

	/************************************ 状态识别 ************************************/
	/************************************ 状态识别 ************************************/
	int32_t abs_out_electric = abs(m_foc_current);
	//堵转检测
	if( (m_mode_run == Motor_Mode_Digital_Current)	//电流模式
	 	&& (abs_out_electric != 0)						//有输出电流
	 	&& (abs(m_est_speed) < (Move_Pulse_NUM/5))		//低于1/5转/s
	){
		if(m_stall_time_us >= (1000 * 1000))	m_stall_flag = true;
		else									m_stall_time_us += CONTROL_PERIOD_US;
	}
	else if( (abs_out_electric == Current_Rated_Current)	//额定电流
			 && (abs(m_est_speed) < (Move_Pulse_NUM/5))		//低于1/5转/s
	){
		if(m_stall_time_us >= (1000 * 1000))	m_stall_flag = true;
		else									m_stall_time_us += CONTROL_PERIOD_US;
	}
	else{
		m_stall_time_us = 0;
		//堵转标志不能自清除，需要外部指令才能清除
	}

	//过载检测
	if(abs_out_electric == Current_Rated_Current){		//额定电流
		if(m_overload_time_us >= (1000 * 1000))	m_overload_flag = true;
		else									m_overload_time_us += CONTROL_PERIOD_US;
	}
	else{
		m_overload_time_us = 0;
		m_overload_flag = false;//过载标志可自清除
	}
	
	// /************************************ 状态记录 ************************************/
	// /************************************ 状态记录 ************************************/
	//统一的电机状态
	if(m_mode_run == Control_Mode_Stop)	//停止模式
		m_state = Control_State_Stop;
	else if(m_stall_flag)								//堵转标志置位
		m_state = Control_State_Stall;
	else if(m_overload_flag)						//过载标志置位
		m_state = Control_State_Overload;
	else
	{
		if(m_mode_run == Motor_Mode_Digital_Location){
			if( (m_soft_location == m_goal_location)
			 && (m_soft_speed == 0))
				m_state = Control_State_Finish;		//软硬目标匹配
			else
				m_state = Control_State_Running;
		}
		else if(m_mode_run == Motor_Mode_Digital_Speed){
			if(m_soft_speed == m_goal_speed)
				m_state = Control_State_Finish;		//软硬目标匹配
			else
				m_state = Control_State_Running;
		}
		else if(m_mode_run == Motor_Mode_Digital_Current){
			if(m_soft_current == m_goal_current)
				m_state = Control_State_Finish;		//软硬目标匹配
			else
				m_state = Control_State_Running;
		}
		else{
			m_state = Control_State_Finish;			//软硬目标匹配
		}
	}
}

/**
  * @brief  设清除积分
  * @param  NULL
  * @retval NULL
**/
void Controller::Clear_Integral(void)
{
	//在此处清除整个控制段代码中的积分项
	m_pid.Clear_Integral();
	m_dce.Clear_Integral();
}

/**
  * @brief  清除堵转识别
  * @param  NULL
  * @retval NULL
**/
void Controller::Clear_Stall(void)
{
	m_stall_time_us = 0;		//堵转计时器
	m_stall_flag = false;		//堵转标志
}

/**
  * @brief  超前角补偿
  * @param  _speed:补偿速度
  * @retval 补偿角度
**/
int32_t Controller::AdvanceCompen(int32_t _speed)
{
	/******************** !!!!! 重要1：本补偿表提取自DPS系列代码                                                  !!!!! ********************/
	/******************** !!!!! 重要2：由于源于其他传感器数据，本补偿表并不完全适合TLE5012和MT6816                !!!!! ********************/
	/******************** !!!!! 重要3：由于测量传感器的最佳补偿曲线十分费时繁琐，作者并不准备在近期进行校准表测量 !!!!! ********************/

	int32_t compen;
	if(_speed < 0){
		if(_speed > -100000)				compen = 0;
		else if(_speed > -1300000)	compen = (((_speed +  100000) * 262) >> 20) -   0;
		else if(_speed > -2200000)	compen = (((_speed + 1300000) * 105) >> 20) - 300;
		else												compen = (((_speed + 2200000) *  52) >> 20) - 390;
		if(compen < -430)						compen = -430;
	}
	else{
		if(_speed < 100000)					compen = 0;																					//(      0,  0) ~ ( 100000,  0)
		else if(_speed <  1300000)	compen = (((_speed -  100000) * 262) >> 20) +   0;	//( 100000,  0) ~ (1300000,300)
		else if(_speed <  2200000)	compen = (((_speed - 1300000) * 105) >> 20) + 300;	//(1300000,300) ~ (2200000,390)
		else												compen = (((_speed - 2200000) *  52) >> 20) + 390;	//(2200000,390) ~ 
		if(compen > 430)						compen = 430;
	}
	return compen;
}