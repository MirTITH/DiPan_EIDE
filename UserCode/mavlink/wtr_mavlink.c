/**
 * @file wtr_mavlink.c
 * @author TITH (1023515576@qq.com)
 * @brief 移植到 stm32 的 mavlink
 * @version 0.2
 * @date 2022-07-15
 *
 * @copyright Copyright (c) 2022
 *
 */

#include "wtr_mavlink.h"

WTR_MAVLink_handle_t hMAVLink[MAVLINK_COMM_NUM_BUFFERS];

/**
 * @brief 查找 hMAVLink
 *
 * @param huart
 * @return WTR_MAVLink_handle_t* 找不到返回 NULL
 */
inline WTR_MAVLink_handle_t *Find_MAVLink_COMM(UART_HandleTypeDef *huart)
{
	for (int i = 0; i < MAVLINK_COMM_NUM_BUFFERS; i++)
	{
		if (huart == hMAVLink[i].huart)
			return &hMAVLink[i];
	}
	return NULL;
}

inline void WTR_MAVLink_Rcv(WTR_MAVLink_handle_t *hmav)
{
	HAL_UART_Receive_IT(hmav->huart, &hmav->rx_ch, 1);
}

/**
 * @brief mavlink 串口发送。mavlink 默认有4个通道，在这里指定每个通道用哪个串口发送
 *
 * @param chan
 * @param buf
 * @param len
 */
void WTR_MAVLINK_SEND_UART_BYTES(mavlink_channel_t chan, const uint8_t *buf, uint16_t len)
{
	UART_HandleTypeDef *huart = NULL;

	if (chan < MAVLINK_COMM_NUM_BUFFERS)
	{
		huart = hMAVLink[chan].huart;
	}

	if (huart != NULL)
	{
		HAL_UART_Transmit(huart, (uint8_t *)buf, len, HAL_MAX_DELAY);
	}
}

/**
 * @brief MAVLink 消息接收完毕回调函数，需要在这里调用解码函数
 *
 * @param msg
 * @return void
 */
__weak void WTR_MAVLink_Msg_RxCpltCallback(mavlink_message_t *msg)
{
	switch (msg->msgid)
	{
	case 1:
		// id = 1 的消息对应的解码函数
		break;

	case 2:
		// id = 2 的消息对应的解码函数
		break;
	// ......
	default:
		break;
	}
}

/**
 * @brief 接收回调，放在串口接收回调函数中
 *
 * @param huart
 */
void WTR_MAVLink_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	WTR_MAVLink_handle_t *hmav = Find_MAVLink_COMM(huart);
	if (hmav == NULL)
		return;

	if (mavlink_parse_char(hmav->chan, hmav->rx_ch, &hmav->msg, &hmav->status))
	{
		WTR_MAVLink_Msg_RxCpltCallback(&hmav->msg);
	}

	HAL_UART_Receive_IT(hmav->huart, &hmav->rx_ch, 1);
}

/**
 * @brief 初始化收发通道
 *
 * @param huart
 * @param chan
 */
void WTR_MAVLink_Init(UART_HandleTypeDef *huart, mavlink_channel_t chan)
{
	hMAVLink[chan].huart = huart;
	hMAVLink[chan].chan = chan;
}

/**
 * @brief
 *
 * @param huart
 * @param chan
 * @param msg_struct
 */
void WTR_MAVLink_RcvStart(mavlink_channel_t chan)
{
	HAL_UART_Receive_IT(hMAVLink[chan].huart, &hMAVLink[chan].rx_ch, 1);
}
