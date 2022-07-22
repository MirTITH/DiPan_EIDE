/**
 * @file controller_data_sender.c
 * @author TITH (1023515576@qq.com)
 * @brief 
 * @version 0.1
 * @date 2022-07-22
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#include "ctrl_data_sender.h"

mavlink_channel_t CtrlDataSendChan = 0;

void CtrlDataSender_Init(UART_HandleTypeDef *huart, mavlink_channel_t chan)
{
	WTR_MAVLink_Init(huart, chan);
	CtrlDataSendChan = chan;
}

void CtrlDataSender_Task(void const *argument)
{
	while (1)
	{
		mavlink_msg_controller_send_struct(CtrlDataSendChan, argument);
		osDelay(10);
	}
}

void CtrlDataSender_Start(mavlink_controller_t* controller)
{
	osThreadDef(CtrlSend, CtrlDataSender_Task, osPriorityNormal, 0, 512);
	osThreadCreate(osThread(CtrlSend), controller);
}
