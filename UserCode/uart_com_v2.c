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
#include <string.h>

const uint8_t Frame_Head[FRAME_HEAD_SIZE] = FRAME_HEAD_DEFINE;

/**
 * Name:    CRC-8               x8+x2+x+1
 * Poly:    0x07
 * Init:    0x00
 * Refin:   False
 * Refout:  False
 * Xorout:  0x00
 * Note:
 */
uint8_t crc8(uint8_t *data, uint16_t length)
{
    uint8_t i;
    uint8_t crc = 0;        // Initial value
    while(length--)
    {
        crc ^= *data++;        // crc ^= *data; data++;
        for ( i = 0; i < 8; i++ )
        {
            if ( crc & 0x80 )
                crc = (crc << 1) ^ 0x07;
            else
                crc <<= 1;
        }
    }
    return crc;
}

void UC_Receive(UC_RxInfoHandle_t *hRxInfo, uint8_t *pData, uint16_t Size)
{
	if(hRxInfo->use_dma)
	{
		HAL_UART_Receive_DMA(hRxInfo->huart, pData, Size);
	}
	else
	{
		HAL_UART_Receive_IT(hRxInfo->huart, pData, Size);
	}
}

inline void UC_Receive_ForceIT(UC_RxInfoHandle_t *hRxInfo, uint8_t *pData, uint16_t Size)
{
	HAL_UART_Receive_IT(hRxInfo->huart, pData, Size);
}

void DataPacket_Rcv(UC_RxInfoHandle_t *hRxInfo, UC_DataPktHandle_t *hpkt, uint8_t id)
{
	if (hpkt->is_init == false)
		return;
	
	if(hpkt->id != id)
		return;

	UC_Receive(hRxInfo, hpkt->buffer, hpkt->effective_data_size + hpkt->effective_data_size + hpkt->crc_size);
}

bool Frame_Info_Check(Frame_Info_t* info)
{
	if(memcmp(&info->frame_head, &Frame_Head, FRAME_HEAD_SIZE) != 0)
		return false;

	if (crc8((uint8_t *)info, sizeof(Frame_Info_t)) != 0)
		return false;

	return true;
}

inline void Frame_InfoMoveForward(Frame_Info_t *info)
{
	memmove(info, ((uint8_t *)info) + 1, sizeof(Frame_Info_t) - 1);
}

__weak void UC_FrameInfo_RxCpltCallback(UC_RxInfoHandle_t *hRxInfo)
{
	switch (hRxInfo->info.id)
	{
	case 1:
		/* code */
		break;
	
	default:

		break;
	}
}

void UC_UART_RxCpltCallback(UART_HandleTypeDef *huart, UC_RxInfoHandle_t *hRxInfo)
{
	if (hRxInfo == NULL)
		return;

	if (hRxInfo->huart != huart)
		return;

	if (hRxInfo->info_rx_cplt == false)
	{
		if (hRxInfo->frame_aligned)
		{
			if (Frame_Info_Check(&hRxInfo->info))
			{
				hRxInfo->info_rx_cplt = true;
			}
			else
			{
				hRxInfo->frame_aligned = false;
				Frame_InfoMoveForward(&hRxInfo->info);
				UC_Receive_ForceIT(hRxInfo, hRxInfo->info_tail, 1);
			}
		}
		else
		{
			if (Frame_Info_Check(&hRxInfo->info))
			{
				hRxInfo->frame_aligned = true;
				hRxInfo->info_rx_cplt = true;
			}
			else
			{
				Frame_InfoMoveForward(&hRxInfo->info);
				UC_Receive_ForceIT(hRxInfo, hRxInfo->info_tail, 1);
			}
		}
	}

	if(hRxInfo->info_rx_cplt)
	{
		UC_FrameInfo_RxCpltCallback(hRxInfo);
	}
}