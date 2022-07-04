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

// 要传输的数据，里面的变量可以自己添加修改（全称：Uart communication data）
typedef struct
{
	int8_t test_int8;
	int8_t test_int82;
} UC_Data_t;

// // 要发送的数据
// extern UC_Data_t uc_send_data;

// 接收到的数据
extern UC_Data_t uc_rcv_data;

void test();