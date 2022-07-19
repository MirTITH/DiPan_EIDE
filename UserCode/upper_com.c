/**
 * @file upper_com.c
 * @author TITH (1023515576@qq.com)
 * @brief 
 * @version 0.1
 * @date 2022-07-16
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#include "upper_com.h"
#include "cmsis_os.h"
#include "uart_device.h"
#include "beep.h"
#include <stdbool.h>

mavlink_upper_t UpperTxData;

#define ServoTypePos 0
#define ServoTypeSpeed 1

typedef struct
{
	float lift_pos[5];
	int now_pos_id; // 0-4
	uint32_t last_up_tick;
	uint32_t last_down_tick;
	uint32_t button_min_time;
}LiftData_t;

LiftData_t lift_data = {
	.last_down_tick = 0,
	.last_up_tick = 0,
	.button_min_time =300,
	.now_pos_id = 0,
	.lift_pos[0] = 0,
	.lift_pos[1] = 1000,
	.lift_pos[2] = 2000,
	.lift_pos[3] = 3000,
	.lift_pos[4] = 4000
};

typedef struct
{
	uint32_t last_tick;
	uint32_t button_min_time;
	float close_pos;
	float open_pos;
	bool is_open;
}ClawData_t;

ClawData_t claw_data = {
	.button_min_time = 500,
	.close_pos = 10,
	.open_pos = 590
};

void UpperComTaskInit()
{
	WTR_MAVLink_Init(&huart1, MAVLINK_COMM_1);
}

void UpperComTask(void const *argument)
{
	mavlink_controller_t *ctrl_data = (mavlink_controller_t *) argument;
	UpperTxData.servo_type = (ServoTypePos << 0) |
							 (ServoTypePos << 1) |
							 (ServoTypePos << 2);

	uint32_t PreviousWakeTime = osKernelSysTick();
	for (;;)
	{
		if (ctrl_data->buttons & (1 << 1)) // 主升降升
		{
			if (lift_data.last_up_tick + lift_data.button_min_time < HAL_GetTick())
			{
				lift_data.last_up_tick = HAL_GetTick();
				if (lift_data.now_pos_id < 4)
				{
					lift_data.now_pos_id++;
					Beep();
				}
			}
		}

		if (ctrl_data->buttons & (1 << 0)) // 主升降降
		{
			if (lift_data.last_down_tick + lift_data.button_min_time < HAL_GetTick())
			{
				lift_data.last_down_tick = HAL_GetTick();
				if (lift_data.now_pos_id > 0)
				{
					lift_data.now_pos_id--;
					Beep();
				}
			}
		}

		UpperTxData.lift = lift_data.lift_pos[lift_data.now_pos_id];

		// if (ctrl_data->buttons & (1 << 3)) // 爪子开合DJI
		// {
		// 	if (claw_data.last_tick + claw_data.button_min_time < HAL_GetTick())
		// 	{
		// 		claw_data.last_tick = HAL_GetTick();
		// 		claw_data.is_open = !claw_data.is_open;
		// 		UD_printf("is_open:%d\n", claw_data.is_open);
		// 	}
		// }
		
		// if (claw_data.is_open)
		// {
		// 	UpperTxData.claw_OC_DJI = claw_data.open_pos;
		// }
		// else
		// {
		// 	UpperTxData.claw_OC_DJI = claw_data.close_pos;
		// }
		

		// UD_printf("lift:%f\n", UpperTxData.lift);
		// UD_printf("claw_OC_DJI:%f\n", UpperTxData.claw_OC_DJI);

		mavlink_msg_upper_send_struct(MAVLINK_COMM_1, &UpperTxData);
		osDelayUntil(&PreviousWakeTime, 2);
	}
}

void UpperComTaskStart(mavlink_controller_t *ctrl_data)
{
	osThreadDef(UpperCom, UpperComTask, osPriorityNormal, 0, 256);
	osThreadCreate(osThread(UpperCom), ctrl_data);
}

