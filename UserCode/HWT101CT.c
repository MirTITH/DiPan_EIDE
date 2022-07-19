/**
 * @file HWT101CT.c
 * @brief 
 * @version 0.1
 * @date 2022-07-14
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include "HWT101CT.h"
#include <string.h>
#include "useful_constant.h"

uint8_t angular_speed_HEAD[HEAD_SIZE] = angular_speedHEAD_DEFINE;
uint8_t angle_HEAD[HEAD_SIZE] = angleHEAD_DEFINE;

inline void HWT_UART_Receive(UART_HandleTypeDef *huart, uint8_t *pData, uint16_t Size)
{
	#ifdef HWT_USE_DMA
		HAL_UART_Receive_DMA(huart, pData, Size);
	#else
		HAL_UART_Receive_IT(huart, pData, Size);
	#endif
}

void HWT_Init(HWT_Handle_t *hhwt, UART_HandleTypeDef *huart)
{
	memset(hhwt, 0, sizeof(HWT_Handle_t));
	hhwt->huart = huart;
	hhwt->frame_aligned = false;
	hhwt->rx_buffer_head_tail = ((uint8_t *)&hhwt->rx_buffer) + HEAD_SIZE - 1;
	HWT_UART_Receive(huart, hhwt->rx_buffer_head_tail, 1);
}

uint8_t SUM_Calc(uint8_t *data, int size)
{
	uint8_t sum = 0;
	for (int i = 0; i < size; i++)
	{
		sum += data[i];
	}
	return sum;
}

int HWT_HeadCheck(HWT_RxBuffer_t *RxBuffer)
{
	if(memcmp(RxBuffer->head, angular_speed_HEAD, HEAD_SIZE) == 0)
		return 1;
	if(memcmp(RxBuffer->head, angle_HEAD, HEAD_SIZE) == 0)
		return 2;

	return 0;
}

void HWT_UART_RxCpltCallback(HWT_Handle_t *hhwt, UART_HandleTypeDef *huart)
{
	if(hhwt->huart->Instance != huart->Instance)
	{
		return;
	}
	
	if(hhwt->frame_aligned)
	{
		switch (HWT_HeadCheck(&hhwt->rx_buffer))
		{
		case 1:
			if (SUM_Calc((uint8_t *)&hhwt->rx_buffer, sizeof(HWT_RxBuffer_t) - 1) == hhwt->rx_buffer.sum)
			{
				hhwt->raw_data.Wz_Raw = hhwt->rx_buffer.data.angular_speed.WzH_Raw << 8 | hhwt->rx_buffer.data.angular_speed.WzL_Raw;
				hhwt->raw_data.Wz_Corrected = hhwt->rx_buffer.data.angular_speed.WzH_Corrected << 8 | hhwt->rx_buffer.data.angular_speed.WzL_Corrected;
			}
			HWT_UART_Receive(hhwt->huart, (uint8_t *)&hhwt->rx_buffer, sizeof(HWT_RxBuffer_t));
			break;
		case 2:
			if (SUM_Calc((uint8_t *)&hhwt->rx_buffer, sizeof(HWT_RxBuffer_t) - 1) == hhwt->rx_buffer.sum)
			{
				hhwt->raw_data.Yaw = hhwt->rx_buffer.data.angle.YawH << 8 | hhwt->rx_buffer.data.angle.YawL;
				hhwt->raw_data.V = hhwt->rx_buffer.data.angle.VH << 8 | hhwt->rx_buffer.data.angle.VL;
			}
			HWT_UART_Receive(hhwt->huart, (uint8_t *)&hhwt->rx_buffer, sizeof(HWT_RxBuffer_t));
			break;

		default:
			hhwt->frame_aligned = false;
			break;
		}
	}

	if (hhwt->frame_aligned == false)
	{
		if(HWT_HeadCheck(&hhwt->rx_buffer) == 0)
		{
			memmove(&hhwt->rx_buffer, ((uint8_t *)&hhwt->rx_buffer) + 1, sizeof(HEAD_SIZE));
			HWT_UART_Receive(hhwt->huart, ((uint8_t *)&hhwt->rx_buffer) + HEAD_SIZE - 1, 1);
		}
		else
		{
			hhwt->frame_aligned = true;
			HWT_UART_Receive(hhwt->huart, ((uint8_t *)&hhwt->rx_buffer) + HEAD_SIZE, sizeof(HWT_RxBuffer_t) - HEAD_SIZE);
		}
	}
}

float HWT_ReadWzDeg(HWT_Handle_t *hhwt)
{
	return hhwt->raw_data.Wz_Corrected * (2000.0 / 32768.0);
}

float HWT_ReadYawDeg(HWT_Handle_t *hhwt)
{
	return hhwt->raw_data.Yaw * (180.0 / 32768.0);
}

float HWT_ReadWzRad(HWT_Handle_t *hhwt)
{
	return hhwt->raw_data.Wz_Corrected * (2000.0 * M_PI / 180 / 32768.0);
}

float HWT_ReadYawRad(HWT_Handle_t *hhwt)
{
	return hhwt->raw_data.Yaw * (180.0 * M_PI / 180 / 32768.0);
}