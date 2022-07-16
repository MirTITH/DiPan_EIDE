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

void UpperComTaskInit()
{
	WTR_MAVLink_Init(&huart3, 1);
}

void UpperComTask(void const *argument)
{
	uint32_t PreviousWakeTime = osKernelSysTick();
	UpperTxData.claw_OC_L = 500;
	UpperTxData.claw_OC_R = 1000;
	for (;;)
	{
		UpperTxData.claw_OC_R++;
		mavlink_msg_upper_send_struct(1, &UpperTxData);
		osDelayUntil(&PreviousWakeTime, 1);
	}	
}

void UpperComTaskStart(void *argument)
{
	osThreadDef(UpperCom, UpperComTask, osPriorityNormal, 0, 256);
	osThreadCreate(osThread(UpperCom), argument);
}

