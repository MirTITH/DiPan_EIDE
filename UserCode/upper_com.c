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
	.lift_pos[0] = 1000,
	.lift_pos[1] = 2000,
	.lift_pos[2] = 3000,
	.lift_pos[3] = 4000,
	.lift_pos[4] = 5000
};

float lift_pos[5] = {
	1000,
	2000,
	3000,
	4000,
	5000
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

		// UD_printf("lift:%f\n", UpperTxData.lift);

		mavlink_msg_upper_send_struct(MAVLINK_COMM_1, &UpperTxData);
		osDelayUntil(&PreviousWakeTime, 2);
	}
}

void UpperComTaskStart(mavlink_controller_t *ctrl_data)
{
	osThreadDef(UpperCom, UpperComTask, osPriorityNormal, 0, 256);
	osThreadCreate(osThread(UpperCom), ctrl_data);
}

