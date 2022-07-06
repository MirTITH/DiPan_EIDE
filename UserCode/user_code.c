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

extern int fix_counter;

uint16_t rx_data[4] = {0};

UC_Data_t RxData, TxData;

void StartDefaultTask(void const *argument)
{
	
	CLI_Init(&huart2);
	UD_SetPrintfDevice(UD_Find(&huart2));

	// UART_DEVICE* ud_9bit = UD_New(&huart1, 64, 64, UartDevice_IT, UartDevice_IT);
	// UD_Open(ud_9bit);

	UC_Rcv_Start(1, &huart6, &RxData);

	TxData.test_int16 = 0x0;
	TxData.test_int8 = 0;

	strcpy(TxData.cha, "This is a test string to test the uart communication");

	// ADS1256_Init();

	while (1)
	{
		// TxData.test_int16++;
		// TxData.test_int32 += 2;
		// TxData.test_int8--;
		// UC_Send(1, &huart6, &TxData);

		// test();
		// ADS1256_UpdateDiffData();
		// UD_printf("channel:");
		// for (int i = 0; i < 4; i++)
		// {
		// 	UD_printf("%d,", ADS1256_diff_data[i]);
		// }
		// UD_printf("%8d", ADS1256_diff_data[3]);
		// UD_printf("\n");
		// UD_printf("fix counter: %d\n", fix_counter);


		// UD_WriteStr(ud_9bit, (char*)&data, 1, 1000);

		// UD_Read(ud_9bit, &rx_data, portMAX_DELAY);


		
		// HAL_UART_Transmit(&huart6, (uint8_t*)data16, 4, 1000);

		// for (int i = 0; i < 4; i++)
		// {
		// 	UD_printf("%x ", rx_data[i]);
		// }
		
		// UD_printf("\n");

		// UD_printf("crc:%x\n", crc8(data8, 4));

		// data8[4] = crc8(data8, 4);

		// UD_printf("crc check:%x\n", crc8(data8, 5));

		// UD_printf("rx_data: %x %x\n", rx_data[0], rx_data[1]);

		osDelay(500);
	}
}
