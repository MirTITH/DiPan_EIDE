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

float pos[4] = {0};
float buffer[4] = {0};
float rd[4] = {0};
float erpm[4] = {0};
VESC_t hvesc[4];

int ads_read_data_sw = 1; // ADS打印开关

void StartDefaultTask(void const *argument)
{
	// CLI_Init(&huart3);
	// UD_SetPrintfDevice(UD_Find(&huart3));

	osDelay(500);

	//大疆电机初始化
	CANFilterInit(&hcan1);
	hDJI[4].motorType = M2006;
	hDJI[5].motorType = M2006;
	hDJI[6].motorType = M2006;
	hDJI[7].motorType = M2006;
	DJI_Init();

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

		// Kine Read Wheel Data
		for (int i = 0; i < Wheel_Num; i++)
		{
			wheel[i].wheel_now.rot_pos = pos[i] * PI / 600;
			buffer[i] = pos[i];
		}

		//运动解算
		robot_vx = ((float)(2048 - Leftx)) / 500;
		robot_vy = ((float)(2048 - Lefty)) / 500;
		robot_rot = ((float)(Rightx - 2048)) / 500;
		Kine_SetSpeed(robot_vx, robot_vy, robot_rot);

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

			positionServo(pos[i] + rd[i] * 1200, &hDJI[i + 4]);
		}

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

		osDelay(5);
	}
}
