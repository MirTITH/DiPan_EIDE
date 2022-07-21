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
#include "string.h"
#include <stdbool.h>
#include "chassis_control.h"
#include "wtr_mavlink.h"
#include "upper_com.h"
#include "beep.h"

// extern int fix_counter;

bool pnt_UC_Debug_Data = false;
int reboot_counter = 0;

bool Reseting = false;

void TestTask(void const *argument);

mavlink_controller_t ControllerData = {0};

void StartDefaultTask(void const *argument)
{
	CLI_Init(&huart2);
	UD_SetPrintfDevice(UD_Find(&huart2));

	Beep_Init();

	osThreadDef(testTask, TestTask, osPriorityNormal, 0, 512);
	osThreadCreate(osThread(testTask), NULL);

	//大疆电机初始化
	CANFilterInit(&hcan1);
	hDJI[4].motorType = M2006;
	hDJI[5].motorType = M2006;
	hDJI[6].motorType = M2006;
	hDJI[7].motorType = M2006;
	DJI_Init();

	WTR_MAVLink_Init(&huart6, MAVLINK_COMM_0);
	WTR_MAVLink_RcvStart(MAVLINK_COMM_0);
	ChassisTaskStart(&ControllerData);

	UpperComTaskInit();
	UpperComTaskStart(&ControllerData);
	// ADS1256_Init();

	while (1)
	{
		if ((ControllerData.buttons & (1 << 6)) && (ControllerData.buttons & (1 << 7)))
		{
			reboot_counter++;
			if (reboot_counter > 50)
			{
				BeepSet(0);
				Reseting = true;
				osDelay(500);
				
				HAL_NVIC_SystemReset();
			}

			BeepSet(1);
		}
		else
		{
			if (reboot_counter != 0)
				BeepSet(0);
			reboot_counter = 0;
		}

		osDelay(10);
	}
}

void TestTask(void const *argument)
{
	for (;;)
	{
		if (pnt_UC_Debug_Data)
		{
			// UC_print_debug_data();
			UD_printf("lx:%5d ly:%5d rx:%5d ry:%5d ", ControllerData.left_x, ControllerData.left_y, ControllerData.right_x, ControllerData.right_y);
			UD_printf("but:%x\n", ControllerData.buttons);
		}
		// UD_printf("fix counter: %d\n", fix_counter);

		osDelay(100);
	}
}
