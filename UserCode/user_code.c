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
#include "chassis_control.h"
#include "tim.h"

// extern int fix_counter;

bool pnt_UC_Debug_Data = false;

void TestTask(void const *argument);

UC_Data_t RxData = {0};

int pwmVal = 1000;

void StartDefaultTask(void const *argument)
{
	osDelay(500);
	CLI_Init(&huart2);
	UD_SetPrintfDevice(UD_Find(&huart2));

	HAL_TIM_PWM_Start(&htim4,TIM_CHANNEL_3);

	osThreadDef(testTask, TestTask, osPriorityNormal, 0, 256);
	osThreadCreate(osThread(testTask), NULL);

	while (1)
	{
		__HAL_TIM_SetCompare(&htim4, TIM_CHANNEL_3, pwmVal);
		osDelay(10);
	}
}

void TestTask(void const *argument)
{
	for (;;)
	{
		if (pnt_UC_Debug_Data)
		{
			UC_print_debug_data();
			UD_printf("lx:%5d ly:%5d rx:%5d ry:%5d ", RxData.Leftx, RxData.Lefty, RxData.Rightx, RxData.Righty);
			UD_printf("but:%x\n", RxData.buttons);
		}
		// UD_printf("fix counter: %d\n", fix_counter);
		osDelay(200);
	}
}
