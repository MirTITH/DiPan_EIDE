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

extern bool Reseting;

#define ServoTypePos 0
#define ServoTypeSpeed 1

#define UpperComCycle 10

#define Lift_Pos_Num 5
typedef struct
{
	float lift_pos[Lift_Pos_Num];
	bool is_vice_lift_up[Lift_Pos_Num];
	int now_pos_id; // 0 ~ (Lift_Pos_Num - 1)
	uint32_t last_up_tick;
	uint32_t last_down_tick;
	uint32_t button_min_time;
} LiftData_t;

LiftData_t lift_data = {
	.last_down_tick = 0,
	.last_up_tick = 0,
	.button_min_time = 500,
	.now_pos_id = 0,
	.lift_pos[0] = 0,
	.lift_pos[1] = 2850,
	.lift_pos[2] = 4650,
	.lift_pos[3] = 6450,
	.lift_pos[4] = 6300,
	.is_vice_lift_up[0] = false,
	.is_vice_lift_up[1] = false,
	.is_vice_lift_up[2] = false,
	.is_vice_lift_up[3] = false,
	.is_vice_lift_up[4] = true
	};

typedef struct
{
	uint32_t last_tick;
	uint32_t button_min_time;
	float close_pos;
	float open_pos;
	bool is_open;
} ClawData_t;

ClawData_t claw_data = {
	.is_open = false,
	.button_min_time = 500,
	.close_pos = 10,
	.open_pos = 585};

// typedef struct
// {
// 	int32_t L_open_PW;
// 	int32_t L_close_PW;
// 	int32_t R_open_PW;
// 	int32_t R_close_PW;
// 	uint32_t L_open_tick; // 左舵机打开的时刻
// 	uint32_t R_open_tick; // 右舵机打开的时刻
// } SteerEngine_t;

// SteerEngine_t steer_engine = {
// 	.L_close_PW = 1200, // 1076
// 	.L_open_PW = 1600,	// 1694
// 	.R_close_PW = 1300, // 1195
// 	.R_open_PW = 1600,	// 1767

// 	.L_open_tick = 1000,
// 	.R_open_tick = 2000};

typedef struct
{
	uint32_t last_tick;
	uint32_t button_min_time;
	float pos_turn_over; // 爪子翻转时的位置（相对于正面朝上）
	float pos_offset; // 上电时爪子相对正面朝上时的位置
	bool is_face_up; // 正面朝上
} ClawSpin_t;

ClawSpin_t claw_spin = {
	.button_min_time = 500,
	.is_face_up = 1,
	.last_tick = 0,
	.pos_offset = 0,
	.pos_turn_over = 330
};

void UpperComTaskInit()
{
	WTR_MAVLink_Init(&huart1, MAVLINK_COMM_1);
}

void UpperComTask(void const *argument)
{
	mavlink_controller_t *ctrl_data = (mavlink_controller_t *)argument;
	UpperTxData.servo_type = (ServoTypePos << 0) |
							 (ServoTypePos << 1) |
							 (ServoTypePos << 2);

	uint32_t PreviousWakeTime = osKernelSysTick();
	for (;;)
	{
		/* 升降 */
		if ((ctrl_data->buttons & (1 << 1)) || (ctrl_data -> right_y > 1500)) // 主升降升
		{
			if (lift_data.last_up_tick + lift_data.button_min_time < HAL_GetTick())
			{
				lift_data.last_up_tick = HAL_GetTick();
				if (lift_data.now_pos_id < Lift_Pos_Num - 1)
				{
					lift_data.now_pos_id++;
					Beep();
				}
			}
		}

		if ((ctrl_data->buttons & (1 << 0)) || (ctrl_data -> right_y < -1500)) // 主升降降
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
		UpperTxData.vice_lift = lift_data.is_vice_lift_up[lift_data.now_pos_id]; // 副升降
		

		/* 爪子开合DJI */
		if (ctrl_data->buttons & (1 << 2))
		{
			if (claw_data.last_tick + claw_data.button_min_time < HAL_GetTick())
			{
				claw_data.last_tick = HAL_GetTick();
				claw_data.is_open = !claw_data.is_open;
				// UD_printf("is_open:%d\n", claw_data.is_open);
			}
		}

		if (claw_data.is_open)
		{
			UpperTxData.claw_OC_DJI = claw_data.open_pos;
		}
		else
		{
			UpperTxData.claw_OC_DJI = claw_data.close_pos;
		}

		// /* 爪子舵机 */
		// if (HAL_GetTick() > steer_engine.L_open_tick)
		// {
		// 	UpperTxData.claw_OC_L = steer_engine.L_open_PW;
		// }
		// else
		// {
		// 	UpperTxData.claw_OC_L = steer_engine.L_close_PW;
		// }

		// if (HAL_GetTick() > steer_engine.R_open_tick)
		// {
		// 	UpperTxData.claw_OC_R = steer_engine.R_open_PW;
		// }
		// else
		// {
		// 	UpperTxData.claw_OC_R = steer_engine.R_close_PW;
		// }

		/* 爪子旋转 */
		if (ctrl_data->buttons & (1 << 3))
		{
			if (UpperTxData.lift >= 3000) // 爪子太低时不允许旋转
			{
				if (claw_spin.last_tick + claw_spin.button_min_time < HAL_GetTick())
				{
					claw_spin.last_tick = HAL_GetTick();
					claw_spin.is_face_up = !claw_spin.is_face_up;
				}
			}
		}

		if (ctrl_data->buttons & (1 << 4))
		{
			claw_spin.pos_offset -= 20 * (UpperComCycle / 1000.0);
		}

		if (ctrl_data->buttons & (1 << 5))
		{
			claw_spin.pos_offset += 20 * (UpperComCycle / 1000.0);
		}

		if (claw_spin.is_face_up)
		{
			UpperTxData.claw_spin = -claw_spin.pos_offset;
		}
		else
		{
			UpperTxData.claw_spin = -claw_spin.pos_offset + claw_spin.pos_turn_over;
		}

		// UD_printf("lift:%f\n", UpperTxData.lift);
		// UD_printf("claw_OC_DJI:%f\n", UpperTxData.claw_OC_DJI);
		if(Reseting == false)
		{
			mavlink_msg_upper_send_struct(MAVLINK_COMM_1, &UpperTxData);
		}
		else
		{
			mavlink_msg_upper_send(MAVLINK_COMM_1, 0x00, 3000, 0, 0, 0, 0, 0);
		}
		osDelayUntil(&PreviousWakeTime, UpperComCycle);
	}
}

void UpperTestTask(void const *argument)
{
	while (1)
	{
		UD_printf("claw(DJI:%g L:%d R:%d spin:%g) lift:%g vice:%u type:%x\n",
				  UpperTxData.claw_OC_DJI,
				  UpperTxData.claw_OC_L,
				  UpperTxData.claw_OC_R,
				  UpperTxData.claw_spin,
				  UpperTxData.lift,
				  UpperTxData.vice_lift,
				  UpperTxData.servo_type);
		osDelay(200);
	}
}

void UpperComTaskStart(mavlink_controller_t *ctrl_data)
{
	osThreadDef(UpperCom, UpperComTask, osPriorityNormal, 0, 1024);
	osThreadCreate(osThread(UpperCom), ctrl_data);

	osThreadDef(UpperTest, UpperTestTask, osPriorityNormal, 0, 512);
	osThreadCreate(osThread(UpperTest), NULL);
}
