/**
 * @file wtr_mavlink.h
 * @author TITH (1023515576@qq.com)
 * @brief 移植到 stm32 的 mavlink，只需要包含这个头文件就够了
 * @version 0.2
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

typedef struct
{
	UART_HandleTypeDef *huart;
	uint8_t rx_ch;
	mavlink_channel_t chan;
	mavlink_message_t msg;
	mavlink_status_t status;
} WTR_MAVLink_handle_t;

extern WTR_MAVLink_handle_t hMAVLink[MAVLINK_COMM_NUM_BUFFERS];

void WTR_MAVLink_Init(UART_HandleTypeDef *huart, mavlink_channel_t chan);
void WTR_MAVLink_RcvStart(mavlink_channel_t chan);
void WTR_MAVLink_UART_RxCpltCallback(UART_HandleTypeDef *huart);

#ifdef MAVLINK_USE_CONVENIENCE_FUNCTIONS
void WTR_MAVLINK_SEND_UART_BYTES(mavlink_channel_t chan, const uint8_t *buf, uint16_t len);
#define MAVLINK_SEND_UART_BYTES(chan, buf, len) WTR_MAVLINK_SEND_UART_BYTES(chan, buf, len)
#endif // MAVLINK_USE_CONVENIENCE_FUNCTIONS

#include "mavlink.h"
