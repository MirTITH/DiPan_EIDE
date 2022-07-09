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
#include "uart_com.h"
#include "string.h"
#include <stdbool.h>
#include "chassis.h"

// extern int fix_counter;

bool pnt_UC_Debug_Data = true;

void TestTask(void const *argument);

uni_wheel_t wheels[4];

UC_Data_t RxData = {
	.Leftx = 0,
	.Lefty = 0,
	.Rightx = 0,
	.Righty = 0,
	.buttons = 0
};

void ChassisTask(void const *argument)
{

	//大疆电机初始化
	CANFilterInit(&hcan1);
	// hDJI[0].motorType = M3508; // 爪子
	// hDJI[1].motorType = M2006;
	// hDJI[2].motorType = M3508; // 升降
	
	hDJI[4].motorType = M2006;
	hDJI[5].motorType = M2006;
	hDJI[6].motorType = M2006;
	hDJI[7].motorType = M2006;

	DJI_Init();

	Wheels_Init(wheels);

	uint32_t PreviousWakeTime = osKernelSysTick();

	for (;;)
	{
		for (int i = 0; i < 4; i++)
		{
			if ((RxData.Leftx) * (RxData.Leftx) + (RxData.Lefty) * (RxData.Lefty) > 250000)
			{
				Wheel_SetXY(&wheels[i], (RxData.Leftx) / 2048.0, (RxData.Lefty) / 2048.0);
			}
			else
			{
				Wheel_SetXY(&wheels[i], 0, 0);
			}
		}

		Wheels_CalcTransmit(wheels, 4);
		osDelayUntil(&PreviousWakeTime, 2);
		// osDelay(2);
	}	
}

void StartDefaultTask(void const *argument)
{
	CLI_Init(&huart2);
	UD_SetPrintfDevice(UD_Find(&huart2));

	osDelay(500);

	osThreadDef(testTask, TestTask, osPriorityNormal, 0, 256);
	osThreadCreate(osThread(testTask), NULL);

	osThreadDef(chassis, ChassisTask, osPriorityBelowNormal, 0, 256);
	osThreadCreate(osThread(chassis), NULL);

	UC_Receive_Start(1, &huart6, &RxData);
	// ADS1256_Init();

	while (1)
	{
		// if (pnt_UC_Debug_Data)
		// {
		// 	UC_print_debug_data();
		// }
		     

		// ADS1256_UpdateDiffData();
		// UD_printf("channel:");
		// for (int i = 0; i < 4; i++)
		// {
		// 	UD_printf("%d,", ADS1256_diff_data[i]);
		// }
		// UD_printf("%8d", ADS1256_diff_data[3]);
		// UD_printf("\n");
		// UD_printf("fix counter: %d\n", fix_counter);

		osDelay(500);
	}
}

void TestTask(void const *argument)
{
	for (;;)
	{
		// if (pnt_UC_Debug_Data)
		// {
		// 	UC_print_debug_data();
		// }

		// UD_printf("lx:%5d ly:%5d rx:%5d ry:%5d ", RxData.Leftx, RxData.Lefty, RxData.Rightx, RxData.Righty);
		// UD_printf("but:%x\n", RxData.buttons);

			UD_printf("speed:");
			for (int i = 0; i < 4; i++)
			{
				UD_printf("%6.2lf ", wheels[i].exp_speed);
			}

			UD_printf("rot_pos:");
			for (int i = 0; i < 4; i++)
			{
				UD_printf("%6.2lf ", wheels[i].exp_rot_pos);
			}
			UD_printf("\n");

		osDelay(200);
	}
}
