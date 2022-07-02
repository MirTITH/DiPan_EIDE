/**
 * @file uart_device.h
 * @author TITH (1023515576@qq.com)
 * @brief UART设备框架，用于统一管理FreeRTOS下UART的使用，避免多线程下的冲突
 * @version 2.0
 * @date 2022-02-11
 * 
 * 速度测试(v 1.3)
 * 传输 (115200 / 8) Bytes 数据所用时间(ms)，波特率 2000000
 * 
 * 一次传输整个字符串：
 * printf("%s")               92
 * HAL_UART_Transmit()        72
 * UD_WriteStr()      72
 * UD_WriteStrCopy()  72
 * 
 * 一次传输一个字符：
 * HAL_UART_Transmit()        86
 * UD_WriteChar()     115
 *
 * @copyright Copyright (c) 2022
 *
 */

#pragma once

#include "cmsis_os.h"
#include "usart.h"

typedef struct
{
	UART_HandleTypeDef *huart;
	uint8_t is_open;
	uint16_t word_length;

	char *tx_buffer;
	uint16_t *tx_buffer_9bit;
	uint16_t tx_buffer_length;
	SemaphoreHandle_t tx_sem;
	BaseType_t (*TxFunc)(UART_HandleTypeDef *huart, const char *pData, uint16_t size);
	BaseType_t (*TxFuncBlock)(UART_HandleTypeDef *huart, const char *pData, uint16_t size, uint32_t timeout);

	uint16_t rx_temp_buffer;
	uint16_t rx_queue_length;
	QueueHandle_t rx_queue;
	BaseType_t (*RxFunc)(UART_HandleTypeDef *huart, char *pData, uint16_t size);
	osThreadId thread_id;
} UART_DEVICE;

typedef enum
{
	UartDevice_IT = 1,
	UartDevice_DMA = 2
} UartDevice_Mode;

/**
 * @brief 新建串口设备（之后还需打开串口才能收发信息）
 * 
 * @param huart 串口句柄
 * @param tx_buffer_length 发送缓冲区长度（设为0表示不使用发送缓冲区）
 * @param rx_queue_length 接收缓冲区长度（设为0表示不使用接收缓冲区）
 * @param tx_mode 发送模式（建议 UartDevice_DMA ）
 * @param rx_mode 接收模式（建议 UartDevice_IT ）
 * @return UART_DEVICE* 对应的UART_DEVICE(失败则返回NULL)
 */
UART_DEVICE *UD_New(UART_HandleTypeDef *huart, uint16_t tx_buffer_length, uint16_t rx_queue_length, UartDevice_Mode tx_mode, UartDevice_Mode rx_mode);

/**
 * @brief 查找对应的UART_DEVICE
 *
 * @param huart 串口句柄
 * @return UART_DEVICE* 对应的查找对应的UART_DEVICE指针（未找到返回NULL)
 */
UART_DEVICE *UD_Find(UART_HandleTypeDef *huart);

/**
 * @brief 删除UartDevice，释放内存。确保删除时该设备未使用
 *
 * @param uart_device
 */
void UD_Del(UART_DEVICE *uart_device);

/**
 * @brief 打开串口设备
 * 
 * @param uart_device 串口设备
 * @return BaseType_t 成功：pdPASS；失败：pdFAIL
 */
BaseType_t UD_Open(UART_DEVICE *uart_device);

/**
 * @brief 关闭串口设备
 * 
 * @param uart_device 串口设备
 */
void UD_Close(UART_DEVICE *uart_device);

/**
 * @brief 向串口设备写入字符串（不需要发送缓冲区，但需保证传入的字符串在发送完毕前不被修改）
 * 
 * @param uart_device 串口设备
 * @param str 要写入的字符串
 * @param length 字符串的长度
 * @param timeout 当串口被占用时，最大等待时间
 * @return BaseType_t 成功：pdPASS；失败：pdFAIL
 */
BaseType_t UD_WriteStr(UART_DEVICE *uart_device, const char *str, uint16_t length, uint32_t timeout);

/**
 * @brief 以复制方式向串口设备写入字符串（需要发送缓冲区，会将传入的字符串复制到发送缓冲区，然后再发送）
 * 
 * @param uart_device 串口设备
 * @param str 要写入的字符串
 * @param length 字符串的长度（应小于发送缓冲区的长度，否则会被截断）
 * @param timeout 当串口被占用时，最大等待时间
 * @return BaseType_t 成功：pdPASS；失败：pdFAIL
 */
BaseType_t UD_WriteStrCopy(UART_DEVICE *uart_device, const char *str, uint16_t length, uint32_t timeout);

/**
 * @brief 向串口设备写入单个字符
 * 
 * @param uart_device 串口设备
 * @param cha 要写入的字符
 * @param timeout 当串口被占用时，最大等待时间
 * @return BaseType_t 成功：pdPASS；失败：pdFAIL
 */
BaseType_t UD_WriteChar(UART_DEVICE *uart_device, char cha, uint32_t timeout);

/**
 * @brief 等待串口设备发送完毕
 *
 * @param huart 串口句柄
 */
void UD_Sync(UART_DEVICE *uart_device);

/**
 * @brief 读取串口设备
 * 
 * @param uart_device 串口设备
 * @param read_buffer 读取到的数据会被存入 read_buffer（目前只能一次读取一个char）
 * @param timeout 最大等待时间
 * @return BaseType_t 
 */
BaseType_t UD_Read(UART_DEVICE *uart_device,void *const read_buffer,uint32_t timeout);

/**
 * @brief 请在HAL_UART_TxCpltCallback()中调用此函数
 * 
 * @param huart 串口句柄
 */
void UD_TxCpltCallback(UART_HandleTypeDef *huart);

/**
 * @brief 请在HAL_UART_RxCpltCallback()中调用此函数
 * 
 * @param huart 串口句柄
 */
void UD_RxCpltCallback(UART_HandleTypeDef *huart);

/**
 * @brief 使用 UartDevice 的 printf ，比普通的更高效且线程安全。需要先设置使用哪个 UartDevice ，并且 txbuff 的长度要合理，否则会截断
 * 
 * @param format 
 * @param ... 
 */
BaseType_t UD_printf(const char *format, ...);

/**
 * @brief 设置 UD_printf 使用的 UartDevice
 * 
 * @param uart_device 
 */
void UD_SetPrintfDevice(UART_DEVICE *uart_device);
