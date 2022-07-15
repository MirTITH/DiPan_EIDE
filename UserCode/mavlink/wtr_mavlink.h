/**
 * @file mavlink_config.h
 * @author TITH (1023515576@qq.com)
 * @brief 移植到 stm32 的 mavlink，只需要包含这个头文件就够了
 * @version 0.1
 * @date 2022-07-15
 *
 * @copyright Copyright (c) 2022
 *
 */

#pragma once

#include "main.h"
#include "mavlink_types.h"
#include "usart.h"

static mavlink_system_t mavlink_system = {
	1, // System ID (1-255)
	1  // Component ID (a MAV_COMPONENT value)
};

#define MAVLINK_USE_CONVENIENCE_FUNCTIONS

#ifdef MAVLINK_USE_CONVENIENCE_FUNCTIONS
#define MAVLINK_SEND_UART_BYTES(chan, buf, len) WTR_MAVLINK_SEND_UART_BYTES(chan, buf, len)

/**
 * @brief mavlink 串口发送。mavlink 默认有4个通道，在这里指定每个通道用哪个串口发送
 * 
 * @param chan 
 * @param buf 
 * @param len 
 */
inline static void WTR_MAVLINK_SEND_UART_BYTES(mavlink_channel_t chan, const uint8_t *buf, uint16_t len)
{
	UART_HandleTypeDef *huart;
	switch (chan)
	{
	case MAVLINK_COMM_0:
		huart = &huart6;
		break;
	case MAVLINK_COMM_1:
		// huart = 
		break;
	case MAVLINK_COMM_2:
		// huart = 
		break;
	case MAVLINK_COMM_3:
		// huart = 
		break;
	default:
		break;
	}
	if (huart != NULL)
	{
		HAL_UART_Transmit(huart, (uint8_t *)buf, len, HAL_MAX_DELAY);
	}
}
#endif // MAVLINK_USE_CONVENIENCE_FUNCTIONS

#include "mavlink.h"
