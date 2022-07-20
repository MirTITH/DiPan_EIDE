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

#define rx_DEADBAND 100.0

uni_wheel_t wheels[4];

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

void ChassisTask(void const *argument)
{
	const mavlink_controller_t *ctrl_data = argument;
	Chassis_Init(wheels);
	double vx, vy, vrow;
	double speed_ratio = 1.0 / 1024.0;
	double spin_ratio = 1.0 / 1024.0;
	double lx, ly, rx;

	osDelay(500);

	uint32_t PreviousWakeTime = osKernelSysTick();

	for (;;)
	{
		DeadBand(ctrl_data->left_x, ctrl_data->left_y, &lx, &ly, 100);
		rx = ctrl_data->right_x;

		vx = lx * speed_ratio;
		vy = ly * speed_ratio;

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

		Chassis_SetSpeed(wheels, 4, vx, vy, vrow);

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
