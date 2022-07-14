/**
 * @file uart_com_v2.h
 * @author TITH (1023515576@qq.com)
 * @brief 
 * @version 0.1
 * @date 2022-07-14
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#pragma once

#include "main.h"
#include <stdint.h>
#include <stdbool.h>

#define FRAME_HEAD_DEFINE {0xAA, 0x93}
#define FRAME_HEAD_SIZE 2

// 数据帧信息区
typedef struct __attribute__((packed))
{
	uint8_t frame_head[FRAME_HEAD_SIZE]; // 帧头
	uint16_t data_size;						// 数据区大小，不包括校验
	uint8_t id;								// 帧Id
	uint8_t crc8;							// 帧信息区校验
} Frame_Info_t;

typedef struct 
{
	Frame_Info_t info;
	UART_HandleTypeDef *huart;
	uint8_t *info_tail;
	bool frame_aligned;
	bool info_rx_cplt;
	bool use_dma;
} UC_RxInfoHandle_t;

typedef struct
{
	bool is_init;
	uint8_t id;
	uint16_t effective_data_size;
	uint8_t reserve_data_size;
	uint8_t crc_size;
	uint8_t *buffer;
	void *dst; // 目的地
} UC_DataPktHandle_t;

void UC_UART_RxCpltCallback(UART_HandleTypeDef *huart, UC_RxInfoHandle_t *hRxInfo);
