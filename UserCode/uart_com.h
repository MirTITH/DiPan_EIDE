/**
 * @file uart_com.h
 * @author TITH (1023515576@qq.com)
 * @brief 串口数据包通讯
 * @version 0.1
 * @date 2022-07-04
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include "main.h"

// 要传输的数据
typedef struct
{
	int8_t test_int8;
	int16_t test_int16;
} UC_Data_t;

void test();

void UC_Rcv_Init(uint8_t ID, UART_HandleTypeDef* huart, UC_Data_t* data_to_receive);

void UC_Send(uint8_t ID, UART_HandleTypeDef *huart, UC_Data_t *data_to_send);
