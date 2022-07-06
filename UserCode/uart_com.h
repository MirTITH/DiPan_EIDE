/**
 * @file uart_com.h
 * @author TITH (1023515576@qq.com)
 * @brief 串口数据包通讯
 * @version 0.2
 * @date 2022-07-04
 * 
 * @copyright Copyright (c) 2022
 * 
 * 使用方法：
 * 发送：
 *     初始化对应串口
 *     修改 UC_Data_t 结构体，里面定义你想发送的数据
 *     使用 UC_Send() 发送
 * 
 * 接收：
 *     初始化对应串口，使能全局中断
 *     修改 UC_Data_t 结构体，里面定义你想接收的数据（结构体必须与发送方的完全一致，顺序也不能改）
 *     将 UC_RxCpltCallback() 添加到 HAL_UART_RxCpltCallback() 中
 *         示例：
 *         void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
 *         {
 *             UC_RxCpltCallback(huart);
 *         }
 * 
 *     调用一次 UC_Rcv_Start() 开始接收，之后会在收到数据包并校验通过时自动更新数据
 */

#include "main.h"

// 要传输的数据
typedef struct
{
	int16_t test_int16;
	int32_t test_int32;
	int8_t test_int8;
	char cha[100];
} UC_Data_t;

/**
 * @brief 开始接收数据。每收到一个完整数据，在校验通过时都会写入 data_to_receive
 * 
 * @param ID 要接收的数据包 ID
 * @param huart 
 * @param data_to_receive 接收到的数据会写在这里
 */
void UC_Rcv_Start(uint8_t ID, UART_HandleTypeDef* huart, UC_Data_t* data_to_receive);

/**
 * @brief 发送数据包
 * 
 * @param ID 数据包 ID
 * @param huart 串口
 * @param data_to_send 要发送的数据
 */
void UC_Send(uint8_t ID, UART_HandleTypeDef *huart, UC_Data_t *data_to_send);

/**
 * @brief 串口接收回调函数
 * 
 * @param huart 
 */
void UC_RxCpltCallback(UART_HandleTypeDef *huart);


// #define UC_DEBUG

typedef struct 
{
	int rx_pkt_total;
	int rx_pkt_data_valid;
	int rx_pkt_data_invalid;
	int rx_pkt_id_valid;
	int rx_frame_head_loss;
	int rx_frame_head_unaligned;
	int rx_frame_head_realigned;
}UC_Debug_t;

#ifdef UC_DEBUG
void UC_print_debug_data();
#endif 