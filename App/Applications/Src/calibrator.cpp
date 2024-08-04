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
  ** @file     : m_encode_cali.c/h
  ** @brief    : 编码器校准
  ** @versions : 1.1.4
  ** @time     : 2019/11/28
  ** @reviser  : unli (HeFei China)
  ** @explain  : null
*****/

//Oneself
#include "calibrator.hpp"

/**
  * 取余,兼容一个周期的负数(核心控制-参数自动转换为无符号整形)
  * @param  NULL
  * @retval NULL
**/
uint32_t Calibrator::CycleRem(uint32_t a,uint32_t b)
{
	return (a+b)%b;
}

/**
  * 取循环差
  * @param  NULL
  * @retval NULL
**/
int32_t Calibrator::CycleSub(int32_t a, int32_t b, int32_t cyc)
{
	int32_t sub_data;

	sub_data = a - b;
	if(sub_data > (cyc >> 1))		sub_data -= cyc;
	if(sub_data < (-cyc >> 1))	sub_data += cyc;
	return sub_data;
}

/**
  * 取循环平均值
  * @param  NULL
  * @retval NULL
**/
int32_t Calibrator::CycleAverage(int32_t a, int32_t b, int32_t cyc)
{
	int32_t sub_data;
	int32_t ave_data;

	sub_data = a - b;
	ave_data = (a + b) >> 1;

	if(abs(sub_data) > (cyc >> 1))
	{
		if(ave_data >= (cyc >> 1))	ave_data -= (cyc >> 1);
		else												ave_data += (cyc >> 1);
	}
	return ave_data;
}

/**
  * 取循环平均值
  * @param  NULL
  * @retval NULL
**/
int32_t Calibrator::CycleDataAverage(uint16_t *data, uint16_t length, int32_t cyc)
{
	int32_t sum_data = 0;	//积分和
	int32_t sub_data;			//差
	int32_t	diff_data;		//微分

	//加载第一值
	sum_data += (int32_t)data[0];
	//加载后面的值
	for(uint16_t i=1; i<length; i++){
		diff_data = (int32_t)data[i];
		sub_data = (int32_t)data[i] - (int32_t)data[0];
		if(sub_data > (cyc >> 1))		diff_data = (int32_t)data[i] - cyc;
		if(sub_data < (-cyc >> 1))	diff_data = (int32_t)data[i] + cyc;
		sum_data += diff_data;
	}
	//计算平均值
	sum_data = sum_data / length;
	//平均值标准化
	if(sum_data < 0)		sum_data += cyc;
	if(sum_data > cyc)	sum_data -= cyc;
	return sum_data;
}

/**
  * @brief  校准器原始数据检查
  * @param  NULL
  * @retval NULL
  */
void Calibrator::Calibration_Data_Check(void)
{
	uint32_t	count;			//用于各个计数
	int32_t		sub_data;		//用于各个算差

	/******************** 检查平均值连续性及方向 ********************/
	//求解平均值数据
	for(count=0; count<Move_Step_NUM+1; count++){
		m_encode_cali.coder_data_f[count] = (uint16_t)CycleAverage((int32_t)m_encode_cali.coder_data_f[count], (int32_t)m_encode_cali.coder_data_r[count], CALI_Encode_Res);
	}
	//平均值数据检查
	sub_data = CycleSub((int32_t)m_encode_cali.coder_data_f[0], (int32_t)m_encode_cali.coder_data_f[Move_Step_NUM-1], CALI_Encode_Res);
	if(sub_data == 0)	{	m_encode_cali.error_code = CALI_Error_Average_Dir; return;	}
	if(sub_data > 0)	{	m_encode_cali.dir = true;	}
	if(sub_data < 0)	{	m_encode_cali.dir = false;	}
	for(count=1; count<Move_Step_NUM; count++)
	{
		sub_data = CycleSub((int32_t)m_encode_cali.coder_data_f[count], (int32_t)m_encode_cali.coder_data_f[count-1], CALI_Encode_Res);
		if(abs(sub_data) > (CALI_Gather_Encode_Res * 3 / 2))	{	m_encode_cali.error_code = CALI_Error_Average_Continuity;	m_encode_cali.error_data = count;	return;	}	//两次数据差过大
		if(abs(sub_data) < (CALI_Gather_Encode_Res * 1 / 2))	{	m_encode_cali.error_code = CALI_Error_Average_Continuity;	m_encode_cali.error_data = count;	return;	}	//两次数据差过小
		if(sub_data == 0)																			{	m_encode_cali.error_code = CALI_Error_Average_Dir;				m_encode_cali.error_data = count;	return;	}
		if((sub_data > 0) && (!m_encode_cali.dir))							{	m_encode_cali.error_code = CALI_Error_Average_Dir;				m_encode_cali.error_data = count;	return;	}
		if((sub_data < 0) && (m_encode_cali.dir))								{	m_encode_cali.error_code = CALI_Error_Average_Dir;				m_encode_cali.error_data = count;	return;	}
	}

	/******************** 全段域校准 完全拟合传感器数据与电机实际相位角非线性关系 ********************/
	//寻找区间下标及阶跃差值
	uint32_t step_num = 0;
	if(m_encode_cali.dir){
		for(count=0; count<Move_Step_NUM; count++){
			sub_data = (int32_t)m_encode_cali.coder_data_f[CycleRem(count+1, Move_Step_NUM)] - (int32_t)m_encode_cali.coder_data_f[CycleRem(count, Move_Step_NUM)];
			if(sub_data < 0){
				step_num++;
				m_encode_cali.rcd_x = count;//使用区间前标
				m_encode_cali.rcd_y = (CALI_Encode_Res-1) - m_encode_cali.coder_data_f[CycleRem(m_encode_cali.rcd_x, Move_Step_NUM)];
			}
		}
		if(step_num != 1){
			m_encode_cali.error_code = CALI_Error_PhaseStep;
			return;
		}
	}
	else{
		for(count=0; count<Move_Step_NUM; count++){
			sub_data = (int32_t)m_encode_cali.coder_data_f[CycleRem(count+1, Move_Step_NUM)] - (int32_t)m_encode_cali.coder_data_f[CycleRem(count, Move_Step_NUM)];
			if(sub_data > 0){
				step_num++;
				m_encode_cali.rcd_x = count;//使用区间前标
				m_encode_cali.rcd_y = (CALI_Encode_Res-1) - m_encode_cali.coder_data_f[CycleRem(m_encode_cali.rcd_x+1, Move_Step_NUM)];
			}
		}
		if(step_num != 1){
			m_encode_cali.error_code = CALI_Error_PhaseStep;
			return;
		}
	}

	//校准OK
	m_encode_cali.error_code = CALI_No_Error;
	return;
}

/**
  * @brief  校准器初始化
  * @param  NULL
  * @retval NULL
  */
void Calibrator::Calibration_Init(void)
{
	//信号
	m_encode_cali.trigger = false;
	m_encode_cali.error_code = CALI_No_Error;
	m_encode_cali.error_data = 0;
	//读取过程
	m_encode_cali.state = CALI_Disable;
	m_encode_cali.out_location = 0;
		//coder_data_f[]
		//coder_data_r[]
		//dir
	//全段域校准过程数据
	m_encode_cali.rcd_x = 0;
	m_encode_cali.rcd_y = 0;
	m_encode_cali.result_num = 0;

	if (!m_motor->isInit()) {
		m_motor->init();
	}
	if (!m_encoder->isInit()) {
		m_encoder->init();
	}
}


/**
  * @brief  校准器中断回调
  * @param  NULL
  * @retval NULL
  */
void Calibrator::Calibration_Interrupt_Callback(void)
{
#define AutoCali_Speed	2		//自动校准速度(主要用于编码器预数据采集)
#define Cali_Speed			1		//校准速度(用于精确的数据采集)

	//唤醒电流控制
	//CurrentControl_OutWakeUp();
	
	//状态变换
	switch(m_encode_cali.state)
	{
		//失能状态
		case CALI_Disable:
			//ELEC_Set_Sleep();
			if(m_encode_cali.trigger)
			{
				m_motor->SetFocCurrentVector(m_encode_cali.out_location, Current_Cali_Current);
				//CurrentControl_Out_FeedTrack(m_encode_cali.out_location, Current_Cali_Current, true, true);
				m_encode_cali.out_location = Move_Pulse_NUM;			//输出到1圈位置
				m_encode_cali.gather_count = 0;										//采集清零
				m_encode_cali.state = CALI_Forward_Encoder_AutoCali;	//--->编码器正转自动校准
				//初始化标志
				m_encode_cali.error_code = CALI_No_Error;
				m_encode_cali.error_data = 0;
				m_encoder->updateAngle();
			}
		break;
		//编码器正转自动校准
		case CALI_Forward_Encoder_AutoCali://正转个1圈 (1 * Motor_Pulse_NUM) -> (2 * Motor_Pulse_NUM)
			m_encode_cali.out_location += AutoCali_Speed;
			m_motor->SetFocCurrentVector(m_encode_cali.out_location, Current_Cali_Current);
			//CurrentControl_Out_FeedTrack(m_encode_cali.out_location, Current_Cali_Current, true, true);
			if(m_encode_cali.out_location == 2 * Move_Pulse_NUM)
			{
				m_encode_cali.out_location = Move_Pulse_NUM;
				m_encode_cali.state = CALI_Forward_Measure;//--->正向测量
			}
			m_encoder->updateAngle();
		break;
		//正向测量
		case CALI_Forward_Measure://(Motor_Pulse_NUM) -> (2 * Motor_Pulse_NUM)
			if((m_encode_cali.out_location % Move_Divide_NUM) == 0)//每到达采集细分量点采集一次数据
			{
				//采集
				m_encode_cali.coder_data_gather[m_encode_cali.gather_count++] = m_encoder->updateAngle();
				if(m_encode_cali.gather_count == Gather_Quantity){
					//记录数据
					m_encode_cali.coder_data_f[(m_encode_cali.out_location - Move_Pulse_NUM) / Move_Divide_NUM]
						= CycleDataAverage(m_encode_cali.coder_data_gather, Gather_Quantity, CALI_Encode_Res);
					//采集计数清零
					m_encode_cali.gather_count = 0;
					//移动位置
					m_encode_cali.out_location += Cali_Speed;
				}
			}
			else{
				//移动位置
				m_encode_cali.out_location += Cali_Speed;
			}	
			m_motor->SetFocCurrentVector(m_encode_cali.out_location, Current_Cali_Current);
			//CurrentControl_Out_FeedTrack(m_encode_cali.out_location, Current_Cali_Current, true, true);
			if(m_encode_cali.out_location > (2 * Move_Pulse_NUM))
			{
				m_encode_cali.state = CALI_Reverse_Ret;//--->反向回退
			}
		break;
		//反向回退
		case CALI_Reverse_Ret://(2 * Motor_Pulse_NUM) -> (2 * Motor_Pulse_NUM + Motor_Divide_NUM * 20)
			m_encode_cali.out_location += Cali_Speed;
			m_motor->SetFocCurrentVector(m_encode_cali.out_location, Current_Cali_Current);
			//CurrentControl_Out_FeedTrack(m_encode_cali.out_location, Current_Cali_Current, true, true);
			if(m_encode_cali.out_location == (2 * Move_Pulse_NUM + Move_Divide_NUM * 20))
			{
				m_encode_cali.state = CALI_Reverse_Gap;//--->反向消差
			}
		break;
		//反向消差
		case CALI_Reverse_Gap://(2 * Motor_Pulse_NUM + Motor_Divide_NUM * 20) -> (2 * Motor_Pulse_NUM)
			m_encode_cali.out_location -= Cali_Speed;
			m_motor->SetFocCurrentVector(m_encode_cali.out_location, Current_Cali_Current);
			//CurrentControl_Out_FeedTrack(m_encode_cali.out_location, Current_Cali_Current, true, true);
			if(m_encode_cali.out_location == (2 * Move_Pulse_NUM))
			{
				m_encode_cali.state = CALI_Reverse_Measure;//--->反向测量
			}
		break;
		//反向测量
		case CALI_Reverse_Measure://(2 * Motor_Pulse_NUM) -> (Motor_Pulse_NUM)
			if((m_encode_cali.out_location % Move_Divide_NUM) == 0)//每到达采集细分量点采集一次数据
			{
				//采集
				m_encode_cali.coder_data_gather[m_encode_cali.gather_count++] = m_encoder->updateAngle();
				if(m_encode_cali.gather_count == Gather_Quantity){
					//记录数据
					m_encode_cali.coder_data_r[(m_encode_cali.out_location - Move_Pulse_NUM) / Move_Divide_NUM]
						= CycleDataAverage(m_encode_cali.coder_data_gather, Gather_Quantity, CALI_Encode_Res);
					//采集计数清零
					m_encode_cali.gather_count = 0;
					//移动位置
					m_encode_cali.out_location -= Cali_Speed;
				}
			}
			else{
				//移动位置
				m_encode_cali.out_location -= Cali_Speed;
			}	
			m_motor->SetFocCurrentVector(m_encode_cali.out_location, Current_Cali_Current);
			//CurrentControl_Out_FeedTrack(m_encode_cali.out_location, Current_Cali_Current, true, true);
			if(m_encode_cali.out_location < Move_Pulse_NUM)
			{
				m_encode_cali.state = CALI_Operation;//--->解算
			}
		break;
		//解算
		case CALI_Operation:
			//进行校准运算中
			m_motor->SetFocCurrentVector(0, 0);
			//CurrentControl_Out_FeedTrack(0, 0, true, true);
		break;
		default:
		break;
	}
}

/**
  * @brief  校准器主程序回调
  * @param  NULL
  * @retval NULL
  */
void Calibrator::Calibration_Loop_Callback(void)
{
	int32_t		data_i32;
	uint16_t	data_u16;
//	uint16_t	data_u8;
	
	//非解算态退出
	if(m_encode_cali.state != CALI_Operation)
		return;
	
	//PWM输出衰减态
	m_motor->setSleep();

	//校准器原始数据检查
	Calibration_Data_Check();
	
	/**********  进行快速表表格建立  **********/
	if(m_encode_cali.error_code == CALI_No_Error)
	{
		// 以阶跃点位0位置细分点，也就是说取的是绝对零点。
		//数据解析
		/******************** 全段域校准 完全拟合传感器数据与电机实际相位角非线性关系 ********************/
		int32_t step_x, step_y;
		m_encode_cali.result_num = 0;
		m_flash_manager.Empty();		//擦除数据区
		m_flash_manager.Begin();		//开始写数据区
		if(m_encode_cali.dir){
			for(step_x = m_encode_cali.rcd_x; step_x < m_encode_cali.rcd_x + Move_Step_NUM + 1; step_x++){
				data_i32 = CycleSub(	m_encode_cali.coder_data_f[CycleRem(step_x+1, Move_Step_NUM)],
															m_encode_cali.coder_data_f[CycleRem(step_x,   Move_Step_NUM)],
															CALI_Encode_Res);
				if(step_x == m_encode_cali.rcd_x){//开始边缘
					for(step_y = m_encode_cali.rcd_y; step_y < data_i32; step_y++){
						data_u16 = CycleRem(	Move_Divide_NUM * step_x + Move_Divide_NUM * step_y / data_i32,
																	Move_Pulse_NUM);
						m_flash_manager.Write_Data16_Append(&data_u16, 1);
						m_encode_cali.result_num++;
					}
				}
				else if(step_x == m_encode_cali.rcd_x + Move_Step_NUM){//结束边缘
					for(step_y = 0; step_y < m_encode_cali.rcd_y; step_y++){
						data_u16 = CycleRem(	Move_Divide_NUM * step_x + Move_Divide_NUM * step_y / data_i32,
																	Move_Pulse_NUM);
						m_flash_manager.Write_Data16_Append(&data_u16, 1);
						m_encode_cali.result_num++;
					}
				}
				else{//中间
					for(step_y = 0; step_y < data_i32; step_y++){
						data_u16 = CycleRem(	Move_Divide_NUM * step_x + Move_Divide_NUM * step_y / data_i32,
																	Move_Pulse_NUM);
						m_flash_manager.Write_Data16_Append(&data_u16, 1);
						m_encode_cali.result_num++;
					}
				}
			}
		}
		else
		{
			for(step_x = m_encode_cali.rcd_x + Move_Step_NUM; step_x > m_encode_cali.rcd_x - 1; step_x--)
			{
				data_i32 = CycleSub(	m_encode_cali.coder_data_f[CycleRem(step_x, Move_Step_NUM)],
															m_encode_cali.coder_data_f[CycleRem(step_x+1, Move_Step_NUM)],
															CALI_Encode_Res);
				if(step_x == m_encode_cali.rcd_x+Move_Step_NUM){//开始边缘
					for(step_y = m_encode_cali.rcd_y; step_y < data_i32; step_y++){
						data_u16 = CycleRem(	Move_Divide_NUM * (step_x+1) - Move_Divide_NUM * step_y / data_i32,
																	Move_Pulse_NUM);
						m_flash_manager.Write_Data16_Append(&data_u16, 1);
						m_encode_cali.result_num++;
					}
				}
				else if(step_x == m_encode_cali.rcd_x){//结束边缘
					for(step_y = 0; step_y < m_encode_cali.rcd_y; step_y++){
						data_u16 = CycleRem(	Move_Divide_NUM * (step_x+1) - Move_Divide_NUM * step_y / data_i32,
																	Move_Pulse_NUM);
						m_flash_manager.Write_Data16_Append(&data_u16, 1);
						m_encode_cali.result_num++;
					}
				}
				else{//中间
					for(step_y = 0; step_y < data_i32; step_y++){
						data_u16 = CycleRem(	Move_Divide_NUM * (step_x+1) - Move_Divide_NUM * step_y / data_i32,
																	Move_Pulse_NUM);
						m_flash_manager.Write_Data16_Append(&data_u16, 1);
						m_encode_cali.result_num++;
					}
				}
			}
		}
		m_flash_manager.End();	//结束写数据区
		
		if(m_encode_cali.result_num != CALI_Encode_Res)
			m_encode_cali.error_code = CALI_Error_Analysis_Quantity;
	}

	//确认校准结果
	if(m_encode_cali.error_code == CALI_No_Error){
		m_encoder->setRectValid(true);
	}
	else{
		m_encoder->setRectValid(false);
		m_flash_manager.Empty();	//清除校准区数据
	}
	
	//运动配置覆盖
	// Signal_MoreIO_Capture_Goal();			//读取信号端口数据以清除校准期间采样的信号
	// motor_control.stall_flag = true;	//触发堵转保护,即校准后禁用运动控制
	m_motor_controller->m_stall_flag = true;
	//清理校准信号
	m_encode_cali.state = CALI_Disable;
	m_encode_cali.trigger = false;			//清除校准触发
}