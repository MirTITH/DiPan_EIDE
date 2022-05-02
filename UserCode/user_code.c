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
#include "user_code.h"
#include "CLI.h"
#include "ADS1256.h"
#include "uart_device.h"
#include "TITH_time.h"

int ads_read_data_sw = 1; // ADS打印开关

void StartDefaultTask(void const *argument)
{
	CLI_Init(&huart3);
	UD_SetPrintfDevice(UD_Find(&huart3));

	osDelay(1000);
	// printf("Init ADS1256\n");
	ADS1256_Init();

	for (;;)
	{
		uint32_t tick1;
		uint32_t tick2;

		if (ads_read_data_sw)
		{
			tick1 = get_time_us();
			ADS1256_UpdateDiffData();
			tick2 = get_time_us();
			
			for(int8_t i = 0; i < 4; i++)
			{
				UD_printf("%8.5lf  ", (double)ADS1256_diff_data[i] / (1 << 23) * 5 * (5.49 + 4.53)/4.53);
			}

			// printf("\n");
			UD_printf("  ADS1256 time:%lu us  ",tick2 - tick1);
			UD_printf("print time:%lu us\n",get_time_us() - tick2);
		}
		else
		{
			ADS1256_UpdateDiffData();
		}

		osDelay(10);
	}
}
