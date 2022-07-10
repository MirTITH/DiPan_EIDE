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

uni_wheel_t wheels[4];

void ChassisTask(void const *argument)
{
	const UC_Data_t *RxData = argument;
	Chassis_Init(wheels);
	double joystick_lx, joystick_ly, joystick_rx, joystick_ry;

	uint32_t PreviousWakeTime = osKernelSysTick();

	for (;;)
	{
		joystick_lx = RxData->Leftx;
		joystick_ly = RxData->Lefty;
		joystick_rx = RxData->Rightx;
		joystick_ry = RxData->Righty;

		if (fabs(joystick_lx) < 500) joystick_lx = 0;
		if (fabs(joystick_ly) < 500) joystick_ly = 0;
		if (fabs(joystick_rx) < 500) joystick_rx = 0;
		if (fabs(joystick_ry) < 500) joystick_ry = 0;

		Chassis_SetSpeed(wheels, 4, joystick_lx / 2048.0, joystick_ly / 2048.0, joystick_rx / 2048.0);

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

void ChassisTaskStart(UC_Data_t *RxData)
{
	osThreadDef(chassis, ChassisTask, osPriorityBelowNormal, 0, 256);
	osThreadCreate(osThread(chassis), RxData);

	osThreadDef(chassis_test, ChassisTestTask, osPriorityBelowNormal, 0, 256);
	osThreadCreate(osThread(chassis_test), NULL);
}
