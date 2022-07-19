/**
 * @file HWT101CT.h
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

#define angular_speedHEAD_DEFINE {0x55, 0x52, 0x00, 0x00} // 角速度输出头
#define angleHEAD_DEFINE {0x55, 0x53, 0x00, 0x00} // 角度输出头
#define HEAD_SIZE 4

#define HWT_USE_DMA

typedef struct __attribute__((packed))
{
	uint8_t head[HEAD_SIZE];
	union
	{
		struct __attribute__((packed))
		{
			uint8_t WzL_Raw;
			uint8_t WzH_Raw;
			uint8_t WzL_Corrected;
			uint8_t WzH_Corrected;
			uint8_t RESERVE_DATA[2];
		}angular_speed;

		struct __attribute__((packed))
		{
			uint8_t RESERVE_DATA[2];
			uint8_t YawL;
			uint8_t YawH;
			uint8_t VL;
			uint8_t VH;
		}angle;
	}data;
	
	uint8_t sum;
}HWT_RxBuffer_t;

typedef struct 
{
	int16_t Wz_Raw; // 角速度Z的原始数据
	int16_t Wz_Corrected; // 角速度Z的原始数据经过校准后得到的角速度Z
	int16_t Yaw; // 偏航角Z
	int16_t V; // 版本号
}HWT_RawData_t;

typedef struct
{
	UART_HandleTypeDef *huart;
	HWT_RxBuffer_t rx_buffer;
	HWT_RawData_t raw_data;
	uint8_t *rx_buffer_head_tail;
	bool frame_aligned; // 帧是否对齐
}HWT_Handle_t;

void HWT_Init(HWT_Handle_t *hhwt, UART_HandleTypeDef *huart);

void HWT_UART_RxCpltCallback(HWT_Handle_t *hhwt, UART_HandleTypeDef *huart);

/**
 * @brief 读取 z 轴角速度
 * 
 * @param hhwt 
 * @return float 角速度 (deg/s)
 */
float HWT_ReadWzDeg(HWT_Handle_t *hhwt);

/**
 * @brief 读取 z 轴 偏航角
 * 
 * @param hhwt 
 * @return float 偏航角 (deg)
 */
float HWT_ReadYawDeg(HWT_Handle_t *hhwt);

/**
 * @brief 读取 z 轴角速度
 * 
 * @param hhwt 
 * @return float 角速度 (rad/s)
 */
float HWT_ReadWzRad(HWT_Handle_t *hhwt);

/**
 * @brief 读取 z 轴 偏航角
 * 
 * @param hhwt 
 * @return float 偏航角 (rad)
 */
float HWT_ReadYawRad(HWT_Handle_t *hhwt);
