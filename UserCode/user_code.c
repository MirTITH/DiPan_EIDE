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
#include "uart_com.h"
#include "string.h"
#include <stdbool.h>

// extern int fix_counter;

bool pnt_UC_Debug_Data = true;

UC_Data_t RxData, TxData;

void TestTask(void const *argument)
{
	strcpy(TxData.test_string, "This is a test string to test the uart communication");
	for (;;)
	{
		TxData.test_int16++;
		TxData.test_int32 += 2;
		TxData.test_int8--;
		UC_Send(1, &huart6, &TxData);
		osDelay(10);
	}
}

void StartDefaultTask(void const *argument)
{
	CLI_Init(&huart2);
	UD_SetPrintfDevice(UD_Find(&huart2));
	osThreadDef(testTask, TestTask, osPriorityNormal, 0, 256);
	osThreadCreate(osThread(testTask), NULL);

	UC_Receive_Start(1, &huart6, &RxData);
	// ADS1256_Init();

	while (1)
	{
		if (pnt_UC_Debug_Data)
		{
			UC_print_debug_data();
		}
		     

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
