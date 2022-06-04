/**
 * @file user_code.c
 * @author TITH (1023515576@qq.com)
 * @brief 用户代码主文件
 * @version 0.1
 * @date 2022-01-12
 *
 * @copyright Copyright (c) 2022
 *
 */

/*
osThreadDef(defaultTask, StartDefaultTask, osPriorityNormal, 0, 256);
defaultTaskHandle = osThreadCreate(osThread(defaultTask), NULL);
*/

#include "user_code.h"
#include "CLI.h"
#include "ADS1256.h"
#include "uart_device.h"
#include "TITH_time.h"
#include <math.h>

#include "DJI.h"
#include "wtr_can.h"
#include "Caculate.h"
#include "vesc.h"
#include "kinematic_calc.h"
#include "nrf_com.h"

//由遥控器直接得到的机器人速度
double robot_vx=0;
double robot_vy=0;
double robot_rot=0;

int b = 0;

double pos[4] = {0};
double buffer[4] = {0};
double rd[4] = {0};
double erpm[4] = {0};
double epos_offset[4] = {0};
VESC_t hvesc[4];

double pos_toggle = 0;
// double pos_zhuazi = 0;
// double pos_shengjiang = 0;
double speed_shengjiang = 0;
double speed_zhuazi = 0;

int ads_read_data_sw = 1; // ADS打印开关

/**
 * @brief 死区限制
 * 
 * @param dead_band 应为正值
 * @param value
 * @return double 当 fabs(value) < deadband 时，否则视输入正负输出 value - deadband 或 value + deadband；
 */
double Deadband(double deadband, double value)
{
	if (fabs(value) <= deadband)
	{
		return 0.0;
	}

	if (value > deadband)
	{
		return value - deadband;
	}
	else
	{
		return value + deadband;
	}
}

/**
 * @brief 循环变量化简
 * 
 * @param cycle 周期
 * @param value 
 * @return double 化简后的值，在[- T / 2, T / 2] 之间
 */
double LoopSimplify(double cycle, double value)
{
	double mod_value = fmod(value , cycle);

	if (mod_value > cycle / 2)
	{
		mod_value -= cycle;
	}

	if (mod_value < -cycle / 2)
	{
		mod_value += cycle;
	}

	return mod_value;
}

/**
 * @brief 
 * 
 * @param cycle 
 * @param value 
 * @return double 化简后的值，在[0, T] 之间
 */
double LoopSimplify_Positive(double cycle, double value)
{
	double mod_value = LoopSimplify(cycle,value);

	if(mod_value<0) mod_value += 600;

	return mod_value;
}

void UART_Send(void const *argument)
{
	while (1)
	{
		UD_printf("sc %.0lf %.3lf\n", speed_zhuazi, speed_shengjiang);
		osDelay(50);
	}
}

void StartDefaultTask(void const *argument)
{
	// CLI_Init(&huart3);
	UD_Open(UD_New(&huart3, 256, 64, UartDevice_DMA, UartDevice_IT));
	UD_SetPrintfDevice(UD_Find(&huart3));

	osThreadDef(sendder, UART_Send, osPriorityNormal, 0, 128);
	osThreadCreate(osThread(sendder), NULL);

	osDelay(500);

	//大疆电机初始化
	CANFilterInit(&hcan1);
	hDJI[0].motorType = M3508; // 爪子
	hDJI[1].motorType = M2006;
	hDJI[2].motorType = M3508; // 升降
	
	hDJI[4].motorType = M2006;
	hDJI[5].motorType = M2006;
	hDJI[6].motorType = M2006;
	hDJI[7].motorType = M2006;
	DJI_Init();

	hDJI[0].speedPID.outputMax = 8000;
	hDJI[2].speedPID.outputMax = 12000;

	// vesc初始化
	Kine_Init(0.8, 0.8, 0.14, 0.14);
	hvesc[0].hcann = &hcan1;
	hvesc[0].controller_id = 0x00;
	hvesc[1].hcann = &hcan1;
	hvesc[1].controller_id = 0x01;
	hvesc[2].hcann = &hcan1;
	hvesc[2].controller_id = 0x02;
	hvesc[3].hcann = &hcan1;
	hvesc[3].controller_id = 0x03;

	//AS69接收初始化
	nrf_receive_init();

	pos[0] = pos[1] = pos[2] = pos[3] = 0;
	buffer[0] = buffer[1] = buffer[2] = buffer[3] = 0;
	rd[0] = rd[1] = rd[2] = rd[3] = 0;

	// printf("Init ADS1256\n");
	// ADS1256_Init();

	for (;;)
	{
		// uint32_t tick1;
		// uint32_t tick2;
		// tick1 = get_time_us();
		// Kine Read Wheel Data
		// for (int i = 0; i < Wheel_Num; i++)
		// {
		// 	wheel[i].wheel_now.rot_pos = LoopSimplify_Positive(1200,hDJI[i].AxisData.AxisAngle_inDegree) * PI / 600;
		// 	buffer[i] = LoopSimplify_Positive(1200,hDJI[i].AxisData.AxisAngle_inDegree);
		// }
		for (int i = 0; i < Wheel_Num; i++)
		{
			wheel[i].wheel_now.rot_pos = pos[i] * PI / 600;
			buffer[i] = pos[i];
		}

		//运动解算
		robot_vx = ((float)(2048 - Leftx)) / 500;
		robot_vy = ((float)(2048 - Lefty)) / 500;

		double v = sqrt(robot_vx * robot_vx + robot_vy * robot_vy);
		if(v > 2)
		{
			robot_vx *= (v - 2) / v;
			robot_vy *= (v - 2) / v;
		}
		else
		{
			robot_vx = 0;
			robot_vy = 0;
		}

		robot_rot = Deadband(0.3 * 2048 / 500, ((float)(Rightx - 2048)) / 500);
		Kine_SetSpeed(robot_vx, robot_vy, robot_rot);

		// if (pos_shengjiang > 1200) pos_shengjiang = 1200;
		// if (pos_shengjiang < 0) pos_shengjiang = 0;

		speed_shengjiang = Deadband(0.3 * 2048 / 3, -(Righty - 2048) / 3.0);
		// UD_printf("sj %lf\n", speed_shengjiang);

		if(button_A || button_B || button_C || button_D || button_E || button_F || button_G || button_H)
		{
			if (button_E)
			{
				// 抓
				speed_zhuazi = 400;
				// UD_printf("catch\n");
			}

			if(button_F)
			{
				// 放
				speed_zhuazi = -400;
				// UD_printf("release\n");
			}
			// UD_printf("Stop\n");
			// UD_printf("%d %d %d %d %d %d %d %d\n", button_A, button_B, button_C, button_D, button_E, button_F, button_G, button_H);
		}
		else
		{
			speed_zhuazi = 0;
			// UD_printf("Stop\n");
		}

		// UD_printf("%d%d%d%d%d%d%d%d\n", button_A, button_B, button_C, button_D, button_E, button_F, button_G, button_H);
		// speed_zhuazi = ;


		//解算数据->输出数据
		for (int i = 0; i < Wheel_Num; i++)
		{
			pos[i] = wheel[i].wheel_exp.rot_pos * 600 / PI;
			erpm[i] = -(1200 * wheel[i].wheel_exp.forward_v) / (0.26 * PI);
			if (i == 1)
				erpm[i] = (1200 * wheel[i].wheel_exp.forward_v) / (0.26 * PI);
		}

		//角度控制接口
		for (int i = 0; i < Wheel_Num; i++)
		{

			if ((pos[i] - buffer[i]) < -1000)
				rd[i]++; //正转一圈
			else if ((pos[i] - buffer[i]) > 1000)
				rd[i]--; //反转一圈

			positionServo(pos[i] + rd[i]*1200 + epos_offset[i] * (1200 / (2 * PI)), &hDJI[i + 4]);
		}
			// UD_printf("%4.0lf\n", pos[0] + rd[0]*1200);

		// positionServo(pos_zhuazi,&hDJI[0]);//pos_zhuazi为正 爪子闭紧
		positionServo(pos_toggle,&hDJI[1]);//360 -> 翻转180度
		// speedServo(speed_zhuazi, &hDJI[0]); // 爪子 为正则爪子闭紧
		// speedServo(speed_shengjiang, &hDJI[2]); // 升降
		// positionServo(pos_shengjiang,&hDJI[2]);

		CanTransmit_DJI_1234(&hcan1,
                             hDJI[0].speedPID.output,
                             hDJI[1].speedPID.output,
                             hDJI[2].speedPID.output,
                             hDJI[3].speedPID.output);

		CanTransmit_DJI_5678(&hcan1,
							 hDJI[4].speedPID.output,
							 hDJI[5].speedPID.output,
							 hDJI[6].speedPID.output,
							 hDJI[7].speedPID.output);

		//速度控制接口
		VESC_CAN_SET_ERPM(&hvesc[0], erpm[0]);
		VESC_CAN_SET_ERPM(&hvesc[1], erpm[1]);
		VESC_CAN_SET_ERPM(&hvesc[2], erpm[2]);
		VESC_CAN_SET_ERPM(&hvesc[3], erpm[3]);
		
		// if (ads_read_data_sw)
		// {
		// 	tick1 = get_time_us();
		// 	ADS1256_UpdateDiffData();
		// 	tick2 = get_time_us();
			
		// 	for(int8_t i = 0; i < 4; i++)
		// 	{
		// 		UD_printf("%8.5lf  ", (double)ADS1256_diff_data[i] / (1 << 23) * 5 * (5.49 + 4.53)/4.53);
		// 	}

		// 	// printf("\n");
		// 	UD_printf("  ADS1256 time:%lu us  ",tick2 - tick1);
		// 	UD_printf("print time:%lu us\n",get_time_us() - tick2);
		// }
		// else
		// {
		// 	ADS1256_UpdateDiffData();
		// }
		// printf("aaa\n");
		// UD_printf("%d %d %d %d\n", HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_9), HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_10), HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_11), HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_12));

		osDelay(2);
	}
}

void UWheels_Hall_Callback(int id)
{
	double hall_angle = 0;
	double exp_angle = (pos[id] + rd[id]*1200) * (2 * PI / 1200);
	switch (id)
	{
	case 0:
		hall_angle = PI / 4;
		break;
	case 1:
		hall_angle = PI / 4;
		break;
	case 2:
		hall_angle = PI / 4;
		break;
	case 3:
		hall_angle = PI / 4;
		break;
	default:
		break;
	}

	epos_offset[id] -= LoopSimplify(2 * PI, hall_angle - exp_angle + hDJI[id + 4].posPID.cur_error * 2 * PI / 1200);
	// printf("N %.1lf D %.1lf\n", exp_angle * (180 / PI), -epos_offset[id] * (180 / PI));
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	switch (GPIO_Pin)
	{
	case GPIO_PIN_9:
		UWheels_Hall_Callback(0);
		break;
	case GPIO_PIN_10:
		UWheels_Hall_Callback(1);
		break;
	case GPIO_PIN_11:
		UWheels_Hall_Callback(2);
		break;
	case GPIO_PIN_12:
		UWheels_Hall_Callback(3);
		break;
	default:
		// printf("EXTI %d\n", GPIO_Pin);
		break;
	}
}
