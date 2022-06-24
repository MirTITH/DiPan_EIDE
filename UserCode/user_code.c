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
#include <stdio.h>

#include "DJI.h"
#include "wtr_can.h"
#include "Caculate.h"
#include "vesc.h"
#include "kinematic_calc.h"
// #include "nrf_com.h"

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

int sem_take_time = 0, sem_give_time = 0;

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

UART_DEVICE* print_device;

int tx_sem_counter = 0;
extern int fix_counter;

void StartDefaultTask(void const *argument)
{
	
	CLI_Init(&huart2);
	print_device = UD_Find(&huart2);
	UD_SetPrintfDevice(print_device);
	
	ADS1256_Init();

	while (1)
	{
		ADS1256_UpdateDiffData();
		UD_printf("channel:");
		for (int i = 0; i < 4; i++)
		{
			UD_printf("%d,", ADS1256_diff_data[i]);
		}
		// UD_printf("%8d", ADS1256_diff_data[3]);
		UD_printf("\n");

		osDelay(20);
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
