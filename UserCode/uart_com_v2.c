/**
 * @file uart_com_v2.c
 * @author TITH (1023515576@qq.com)
 * @brief 
 * @version 0.1
 * @date 2022-07-14
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include "uart_com_v2.h"

typedef struct
{
	uint8_t frame_head[FRAME_HEAD_SIZE]; // 帧头
	uint16_t data_size;					 // 数据区大小，不包括校验
	uint8_t crc8;						 // 帧信息区校验
} Frame_t;

typedef struct
{
	uint8_t *buffer;
	uint8_t *data_dst; // 数据目的地
	uint16_t data_size;
	bool frame_aligned;
	bool data_ready;
	bool use_dma
}UC_RxHandle_t;

typedef struct
{
	UART_HandleTypeDef *huart;
	UC_RxHandle_t hrx;
}UC_Handle_t;

void UC_Pkt_Unpack()

inline void UC_Receive(UC_Handle_t *huc, uint8_t *pData, uint16_t Size)
{
	if(huc->hrx.use_dma)
	{
		HAL_UART_Receive_DMA(huc->huart, pData, Size);
	}
	else
	{
		HAL_UART_Receive_IT(huc->huart, pData, Size);
	}
}

void UC_UART_RxCpltCallback(UC_Handle_t *huc, UART_HandleTypeDef *huart)
{
	if (huc == NULL || huc->huart != huart)
	{
		return;
	}

	if (huc->hrx.frame_aligned)
	{
		if(huc->hrx.data_ready)
		{

		}
	}
	


	
}