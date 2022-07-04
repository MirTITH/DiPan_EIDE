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

// 帧头的个数
#define HEAD_SIZE 2

#define RESERVEDATA_SIZE (sizeof(UC_Data_t) % 4)
#define ALL_DATA_SIZE (sizeof(UC_Data_t) + RESERVEDATA_SIZE)

// UC_Data_t uc_rcv_data;

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

UC_DataPkg_t pkg_to_send = {
	._HEAD = {0xAA, 0x93},
	._ID = 1};

/**
 * @brief 数据包打包并发送
 *
 * @param package
 * @param data_to_pack
 */
void UC_DataPkg_Pack(UC_DataPkg_t *package, UC_Data_t *data_to_pack)
{
	package->uc_data = *data_to_pack;
	// 校验 HEAD 和 ID
	package->_HEAD_CRC = crc8((uint8_t *)package, HEAD_SIZE + sizeof(package->_ID));
	package->_DATA_CRC = HAL_CRC_Calculate(&hcrc, (uint32_t *)&(package->uc_data), ALL_DATA_SIZE / 4);
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
	return (crc8((uint8_t *)package, HEAD_SIZE + sizeof(package->_ID) + sizeof(package->_HEAD_CRC)) == 0x00) &&
		   (HAL_CRC_Calculate(&hcrc, (uint32_t *)&(package->uc_data), (ALL_DATA_SIZE + sizeof(package->_DATA_CRC)) / 4) == 0x00);
}

/**
 * @brief 发送数据
 *
 * @param data_to_send 要发送的数据
 */
void UC_Send(UART_HandleTypeDef *huart, UC_Data_t *data_to_send)
{
	UC_DataPkg_Pack(&pkg_to_send, data_to_send);
	// UD_printf("crc check: %d\n", UC_DataPkg_Check(&pkg_to_send));
	HAL_UART_Transmit(&huart2, (uint8_t *)&pkg_to_send, sizeof(pkg_to_send), portMAX_DELAY);
}

UC_Data_t test_data = {
	.test_int8 = 0x22,
	.test_int82 = 0x33
};

void test()
{
	// UD_printf("pkg size: %d ", sizeof(UC_DataPkg_t));
	// UD_printf("data size: %d\n", sizeof(UC_Data_t));

	UC_Send(&huart6, &test_data);

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
