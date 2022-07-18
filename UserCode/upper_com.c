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
#include "wtr_mavlink.h"
#include "cmsis_os.h"

mavlink_upper_t UpperTxData;

#define ServoTypePos 0
#define ServoTypeSpeed 1

void UpperComTaskInit()
{
	WTR_MAVLink_Init(&huart1, MAVLINK_COMM_1);
}

void UpperComTask(void const *argument)
{
	uint32_t PreviousWakeTime = osKernelSysTick();
	UpperTxData.servo_type = (ServoTypePos << 0) |
							 (ServoTypeSpeed << 1) |
							 (ServoTypePos << 2);
	for (;;)
	{
		mavlink_msg_upper_send_struct(MAVLINK_COMM_1, &UpperTxData);
		osDelayUntil(&PreviousWakeTime, 2);
	}
}

void UpperComTaskStart(void *argument)
{
	osThreadDef(UpperCom, UpperComTask, osPriorityNormal, 0, 256);
	osThreadCreate(osThread(UpperCom), argument);
}

