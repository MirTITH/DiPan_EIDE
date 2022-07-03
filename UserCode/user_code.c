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

extern int fix_counter;

uint16_t rx_data[4] = {0};

void StartDefaultTask(void const *argument)
{
	
	CLI_Init(&huart2);
	UD_SetPrintfDevice(UD_Find(&huart2));

	// UART_DEVICE* ud_9bit = UD_New(&huart1, 64, 64, UartDevice_IT, UartDevice_IT);
	// UD_Open(ud_9bit);

	// uint32_t data = 0x221123F1;

	uint8_t data8[5] = {0x22, 0x11, 0x23, 0xf1,0x00};

	uint16_t data16[4] = {0x001, 0x003, 0x107, 0x177};

	HAL_UART_Receive_IT(&huart6, (uint8_t*)rx_data, 4);

	// uint8_t data_with_crc[4+1];

	

	// ADS1256_Init();

	while (1)
	{
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


		
		HAL_UART_Transmit(&huart6, (uint8_t*)data16, 4, 1000);

		for (int i = 0; i < 4; i++)
		{
			UD_printf("%x ", rx_data[i]);
		}
		
		UD_printf("\n");

		// UD_printf("crc:%x\n", crc8(data8, 4));

		// data8[4] = crc8(data8, 4);

		// UD_printf("crc check:%x\n", crc8(data8, 5));

		// UD_printf("rx_data: %x %x\n", rx_data[0], rx_data[1]);

		osDelay(500);
	}
}
