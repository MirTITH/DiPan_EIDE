/**
 * @file uart_9bit_com.c
 * @author TITH (1023515576@qq.com)
 * @brief 9 bit 串口通信
 * @version 0.1
 * @date 2022-07-02
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include "uart_9bit_com.h"
#include "usart.h"
#include "cmsis_os.h"
#include "crcLib.h"
#include <string.h>

#define Uart9b_huart huart6
#define Uart9b_rx_data_MAXsize 255

uint16_t rx_head_buff[2];
uint8_t rx_data_size = 0;
uint8_t rx_data_start_flag = 0; // 开始接收数据的标识
uint16_t rx_data_16bit[Uart9b_rx_data_MAXsize + 1]; // 校验位要再占用一位
uint8_t* rx_data_8bit;

void Uart9bCom_Transmit(UART_HandleTypeDef *huart, uint16_t data)
{
	HAL_UART_Transmit(huart, (uint8_t*)&data, 1, portMAX_DELAY);
}

void Uart9bCom_Receive_IT(UART_HandleTypeDef *huart, uint8_t *pData, uint16_t Size)
{
	HAL_UART_Receive_IT(huart, pData, Size);
}

/**
 * @brief 校验帧头(crc8)
 * 
 * @param head_buff 
 * @return int 1：通过；0：不通过
 */
int Uart9bCom_HeadCheck(uint16_t* head_buff)
{
	uint8_t head_buff_8bit[2];
	head_buff_8bit[0] = head_buff[0] & 0xff;
	head_buff_8bit[1] = head_buff[1] & 0xff;
	return !crc8(head_buff_8bit, 2);
}

void Uart9bCom_RxCpltCallback(UART_HandleTypeDef *huart)
{
	if (huart->Instance == Uart9b_huart.Instance)
	{
		if (rx_data_start_flag)
		{ // 数据接收完毕
			// 准备接收第二个数据包的帧头
			rx_data_start_flag = 0;
			memset(rx_head_buff, 0, sizeof(rx_head_buff));
			Uart9bCom_Receive_IT(huart, rx_head_buff, 1);

			// 对接收的数据整理
			for (uint16_t i = 0; i < rx_data_size; i++)
			{
				uint8_t *rx_data_temp = (uint8_t *)rx_data_16bit;
				rx_data_temp[i] = rx_data_16bit[i] & 0xff;
			}
		}
		else
		{ // 未开始接收数据
			if (rx_head_buff[0] & (uint16_t)0x100)
			{ // 接收到的第1帧最高位为1：
				if (rx_head_buff[1] == 0)
				{ // 第2帧未接收
					// 接收第2帧
					Uart9bCom_Receive_IT(huart, rx_head_buff + 1, 1);
				}
				else if ((rx_head_buff[1] & 0x100) && Uart9bCom_HeadCheck(rx_head_buff))
				{ // 第2帧已接收，且校验通过
					// 开始接收数据和最后一位校验位
					rx_data_start_flag = 1;
					Uart9bCom_Receive_IT(huart, rx_data_16bit, (rx_head_buff[0] & 0xff) + 1);
				}
				else
				{ // 第2帧已接收，但校验不通过
					memset(rx_head_buff, 0, sizeof(rx_head_buff));
				}
			}
			else
			{ // 接收到的第1帧最高位为0：
				// 说明不是帧头，重新接收
				Uart9bCom_Receive_IT(huart, rx_head_buff, 1);
			}
		}
	}
}

void Uart9bCom_Receive_Init(UART_HandleTypeDef *huart, uint8_t *pData, uint8_t Size)
{
	memset(rx_head_buff, 0, sizeof(rx_head_buff));
	rx_data_size = Size;
	rx_data_start_flag = 0;
	rx_data_8bit = pData;
	Uart9bCom_Receive_IT(huart, rx_head_buff, 1);
}

/**
 * @brief 打包并发送数据包
 * 
 * @param data 要发送的数据
 * @param length 数据长度
 */
void Uart9bCom_Send(void *data, uint8_t length)
{
	uint8_t *tx_data = (uint8_t *)data;

	Uart9bCom_Transmit(&Uart9b_huart, (uint16_t)0x100 | (uint16_t)length);
	Uart9bCom_Transmit(&Uart9b_huart, (uint16_t)0x100 | (uint16_t)crc8(&length, 1));

	for (int16_t i = 0; i < length; i++)
	{
		Uart9bCom_Transmit(&Uart9b_huart, (uint16_t)tx_data[i] & (uint16_t)0xff);
	}

	Uart9bCom_Transmit(&Uart9b_huart, (uint16_t)crc8(tx_data, length) & (uint16_t)0xff);
}
