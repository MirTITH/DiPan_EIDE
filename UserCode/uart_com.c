/**
 * @file uart_com.c
 * @author TITH (1023515576@qq.com)
 * @brief 串口数据包通讯
 * @version 0.1
 * @date 2022-07-04
 *
 * @copyright Copyright (c) 2022
 *
 */

#include "uart_com.h"
#include "uart_device.h"
#include "crcLib.h"
#include "crc.h"
#include <stdbool.h>
#include <string.h>

// #define UC_DATA_PKG_HEAD {0xAA, 0x93} // 帧头


const uint8_t UC_DataPkg_HEAD[] = {0xAA, 0x93};
#define HEAD_SIZE (sizeof(UC_DataPkg_HEAD)) // 帧头的个数
#define RESERVEDATA_SIZE (sizeof(UC_Data_t) % 4)
#define DATA_AND_RESERVE_SIZE (sizeof(UC_Data_t) + RESERVEDATA_SIZE)

// 数据包
typedef struct __attribute__((packed))
{ // 顺序不能乱改
	uint8_t _HEAD[HEAD_SIZE];
	uint8_t _ID;
	uint8_t _HEAD_CRC;
	UC_Data_t uc_data;
	uint8_t _RESERVEDATA[RESERVEDATA_SIZE]; // 用于将要发送的数据补全到 32 bit 的整数倍（CRC32要求数据为32的整数倍）
	uint32_t _DATA_CRC;
} UC_DataPkg_t;

UC_DataPkg_t pkg_to_send; // 要发送的数据会打包到这里

UC_DataPkg_t pkg_rcvd; // 接收到的数据会先放到这里

// bool pkg_data_rcving = false;
bool head_aligned_flag; // 帧头对齐标识
UART_HandleTypeDef* UC_Rx_huart; // 接收数据的串口
UC_Data_t* UC_Data_To_Rcv;
#define SIZE_BEFORE_HEAD_CRC (HEAD_SIZE + sizeof(pkg_to_send._ID))
#define SIZE_BEFORE_ucdata (HEAD_SIZE + sizeof(pkg_to_send._ID) + sizeof(pkg_to_send._HEAD_CRC))
uint8_t pkg_expected_head[SIZE_BEFORE_ucdata];

// #define SIZE_AFTER_ucdata (RESERVEDATA_SIZE + sizeof(pkg_to_send._DATA_CRC))

/**
 * @brief 串口通讯接收初始化（只发送不需要执行此函数）
 * 
 * @param huart 接收数据的串口句柄（传入 NULL 表示去初始化，即不再接收）
 * @param data_to_receive 接收到的数据会写到这里（去初始化时传 NULL）
 */
void UC_Rcv_Init(UART_HandleTypeDef* huart, UC_Data_t* data_to_receive, uint8_t ID)
{
	// if (huart != NULL)
	// {
	// 	pkg_data_rcving = false;
	// 	memcpy(&pkg_expected_head, UC_DataPkg_HEAD, HEAD_SIZE);
	// 	pkg_expected_head[HEAD_SIZE] = ID;
	// 	pkg_expected_head[SIZE_BEFORE_HEAD_CRC] = crc8(pkg_expected_head, SIZE_BEFORE_HEAD_CRC);
	// 	UC_Rx_huart = huart;
	// 	UC_Data_To_Rcv = data_to_receive;
	// 	HAL_UART_Receive_IT(UC_Rx_huart, (uint8_t)&pkg_rcvd, 1);
	// }
	// else
	// {
	// 	HAL_UART_Abort_IT(UC_Rx_huart);
	// 	memcpy(&pkg_expected_head, 0, sizeof(pkg_expected_head));
	// 	UC_Rx_huart = NULL;
	// 	UC_Data_To_Rcv = NULL;
	// 	pkg_data_rcving = false;
	// }
}

/**
 * @brief 数据包打包并发送
 *
 * @param package
 * @param data_to_pack
 */
void UC_DataPkg_Pack(uint8_t ID, UC_DataPkg_t *package, UC_Data_t *data_to_pack)
{
	memcpy(package->_HEAD, UC_DataPkg_HEAD, HEAD_SIZE);
	package->_ID = ID;
	package->uc_data = *data_to_pack;

	// 写入 HEAD 和 ID 的校验值
	package->_HEAD_CRC = crc8((uint8_t *)package, HEAD_SIZE + sizeof(package->_ID));
	package->_DATA_CRC = HAL_CRC_Calculate(&hcrc, (uint32_t *)&(package->uc_data), DATA_AND_RESERVE_SIZE / 4);
}

bool UC_RxDataPkg_HeadCheck(UC_DataPkg_t *package)
{
	if (memcmp(package, pkg_expected_head, SIZE_BEFORE_ucdata) == 0)
	{
		return true;
	}
	else
	{
		return false;
	}
}

/**
 * @brief 检查数据包校验是否通过
 * 
 * @param package 
 * @return true 通过
 * @return false 不通过
 */
bool UC_DataPkg_Check(UC_DataPkg_t *package)
{
	return (crc8((uint8_t *)package, SIZE_BEFORE_ucdata) == 0x00) &&
		   (HAL_CRC_Calculate(&hcrc, (uint32_t *)&(package->uc_data), (DATA_AND_RESERVE_SIZE + sizeof(package->_DATA_CRC)) / 4) == 0x00);
}

void UC_DataPkg_HeadOffset(UC_DataPkg_t *package)
{
	uint8_t *pkg_ptr8 = (uint8_t *)package;
	for (int i = 0; i < SIZE_BEFORE_ucdata - 1; i++)
	{
		pkg_ptr8[i] = pkg_ptr8[i + 1];
	}
}

/**
 * @brief 发送数据
 *
 * @param data_to_send 要发送的数据
 */
void UC_Send(uint8_t ID, UART_HandleTypeDef *huart, UC_Data_t *data_to_send)
{
	UC_DataPkg_Pack(ID, &pkg_to_send, data_to_send);
	// UD_printf("crc check: %d\n", UC_DataPkg_Check(&pkg_to_send));
	HAL_UART_Transmit(&huart2, (uint8_t *)&pkg_to_send, sizeof(pkg_to_send), portMAX_DELAY);
}

/**
 * @brief 串口接收回调函数
 * 
 */
void UC_RxCpltCallback(UART_HandleTypeDef* huart)
{
	if (UC_Rx_huart == NULL)
		return;

	if (huart->Instance == UC_Rx_huart->Instance)
	{
		if(head_aligned_flag)
		{
			if(UC_RxDataPkg_HeadCheck)
			{
				memcpy(UC_Data_To_Rcv, &pkg_rcvd.uc_data, sizeof(UC_Data_t));
				HAL_UART_Receive_IT(UC_Rx_huart, (uint8_t)&pkg_rcvd, sizeof(UC_DataPkg_t));
			}
			
		}

	}
}

void test()
{
	UC_Data_t test_data = {
		.test_int8 = 0x22,
		.test_int82 = 0x33};
	// UD_printf("pkg size: %d ", sizeof(UC_DataPkg_t));
	// UD_printf("data size: %d\n", sizeof(UC_Data_t));

	UC_Send(1, &huart6, &test_data);

	// UD_printf("pos:  ");

	// for (int i = 0; i < sizeof(pkg_to_send); i++)
	// {
	// 	UD_printf("%3d ", i + 1);
	// }
	// UD_printf("\ndata: ");
	// for (int i = 0; i < sizeof(pkg_to_send); i++)
	// {
	// 	UD_printf("%3x ", ((uint8_t*)&pkg_to_send)[i]);
	// }
	// UD_printf("\n");
}
