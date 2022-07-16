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

void UpperComTask(void const *argument)
{
	uint32_t PreviousWakeTime = osKernelSysTick();
	UpperTxData.claw_OC_L = 500;
	UpperTxData.claw_OC_R = 1000;
	for (;;)
	{
		mavlink_msg_upper_send_struct(1, &UpperTxData);
		osDelayUntil(&PreviousWakeTime, 5);
	}	
}

void UpperComTaskStart(void *argument)
{
	osThreadDef(UpperCom, UpperComTask, osPriorityNormal, 0, 256);
	osThreadCreate(osThread(UpperCom), argument);
}

void UpperComTaskInit()
{
	WTR_MAVLink_Init(&huart4, 1);
}