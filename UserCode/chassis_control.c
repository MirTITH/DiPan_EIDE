/**
 * @file chassis_control.c
 * @author TITH (1023515576@qq.com)
 * @brief 底盘控制
 * @version 0.1
 * @date 2022-07-09
 *
 * @copyright Copyright (c) 2022
 *
 */

#include "chassis_control.h"
#include "chassis_driver.h"
#include "cmsis_os.h"
#include "main.h"
#include "DJI.h"
#include "math.h"
#include "uart_device.h"
#include "useful_constant.h"
#include "beep.h"

#define rx_DEADBAND 100.0

uni_wheel_t wheels[4];

double HallCorrectingStartPos[4];
uint32_t HallCorrectingStartTick;

enum
{
	Normal,
	HallCorrecting
} RunningState = Normal;

void DeadBand(double x, double y, double *new_x, double *new_y, double threshould)
{
	double length = sqrt(x * x + y * y);
	double new_length = length - threshould;

	if (new_length <= 0)
	{
		*new_x = 0;
		*new_y = 0;
		return;
	}

	double k = new_length / length;

	*new_x = x * k;
	*new_y = y * k;
}

/**
 * @brief 原地转圈霍尔校准
 * 
 */
void Chassis_HallCorrecting(uni_wheel_t *wheel, int num, uint32_t start_tick)
{
	double pos = 0;

	pos = (double)(HAL_GetTick() - start_tick) * (2 * M_PI / 1000.0);

	for (int i = 0; i < num; i++)
	{
		Wheel_Set(&wheel[i], 0, HallCorrectingStartPos[i] + pos);
	}
	// UD_printf("pos:%f\n", pos);
	
	if (pos > 1.1 * 2 * M_PI)
	{
		RunningState = Normal;
	}
	Wheels_CalcTransmit(wheel, num);
}

#define SpeedRatioNum 2

struct
{
	double ratio[SpeedRatioNum];
	int id;
	uint32_t last_tick;
	uint32_t button_min_time;
} SpeedRatio = {
	.ratio[0] = 1 / 2048.0,
	.ratio[1] = 1 / 1024.0,
	.button_min_time = 500,
	.last_tick = 0,
	.id = 0};

void ChassisTask(void const *argument)
{
	const mavlink_controller_t *ctrl_data = argument;
	Chassis_Init(wheels);
	double vx, vy, vrow;
	double spin_ratio = 1.0 / 1024.0;
	double lx, ly, rx;

	osDelay(500);

	uint32_t PreviousWakeTime = osKernelSysTick();

	for (;;)
	{
		/* 速度切换 */
		if (ctrl_data->buttons & (1 << 7))
		{
			if (SpeedRatio.last_tick + SpeedRatio.button_min_time < HAL_GetTick())
			{
				SpeedRatio.last_tick = HAL_GetTick();
				if (SpeedRatio.id == 0)
				{
					SpeedRatio.id = 1;
					Beep();
					Beep();
				}
				else
				{
					SpeedRatio.id = 0;
					Beep();
				}
			}
		}

		DeadBand(ctrl_data->left_x, ctrl_data->left_y, &lx, &ly, 100);
		rx = ctrl_data->right_x;

		vx = lx * SpeedRatio.ratio[SpeedRatio.id];
		vy = ly * SpeedRatio.ratio[SpeedRatio.id];

		if (fabs(rx) <= rx_DEADBAND)
		{
			vrow = 0;
		}
		else
		{
			if (rx > 0)
			{
				vrow = (rx - rx_DEADBAND) * spin_ratio;
			}
			else
			{
				vrow = (rx + rx_DEADBAND) * spin_ratio;
			}
		}
		if ((ctrl_data->buttons & (1 << 0)) && (RunningState == Normal))
		{
			RunningState = HallCorrecting;
			for (int i = 0; i < 4; i++)
			{
				HallCorrectingStartPos[i] = wheels[i].now_rot_pos;
			}

			HallCorrectingStartTick = HAL_GetTick();
		}

		switch (RunningState)
		{
		case Normal:
			Chassis_SetSpeed(wheels, 4, vx, vy, vrow);
			break;
		case HallCorrecting:
			Chassis_HallCorrecting(wheels, 4, HallCorrectingStartTick);
			break;
		default:
			break;
		}

		osDelayUntil(&PreviousWakeTime, 2);
	}
}

void ChassisTestTask(void const *argument)
{
	while (1)
	{
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

void ChassisTaskStart(mavlink_controller_t *ctrl_data)
{
	osThreadDef(chassis, ChassisTask, osPriorityBelowNormal, 0, 1024);
	osThreadCreate(osThread(chassis), ctrl_data);

	// osThreadDef(chassis_test, ChassisTestTask, osPriorityBelowNormal, 0, 256);
	// osThreadCreate(osThread(chassis_test), NULL);
}
