/**
 * @file uart_device.c
 * @author TITH (1023515576@qq.com)
 * @brief UART设备框架，用于统一管理FreeRTOS下UART的使用，避免多线程下的冲突
 * @version 1.3
 * @date 2022-02-11
 *
 * @copyright Copyright (c) 2022
 *
 */

#include "uart_device.h"
#include <string.h>
#include <stdarg.h>
#include <stdio.h>

#define MAX_UartDevice_Num 4

static UART_DEVICE *uart_devices[MAX_UartDevice_Num] = {NULL};

UART_DEVICE *printf_uart_device = NULL;

void UD_SetPrintfDevice(UART_DEVICE *uart_device)
{
	printf_uart_device = uart_device;
}

BaseType_t UD_printf(const char *format, ...)
{
	if (printf_uart_device == NULL || printf_uart_device->is_open == pdFALSE)
		return pdFAIL;

	if (xSemaphoreTake(printf_uart_device->tx_sem, portMAX_DELAY) == pdPASS)
	{
		va_list args_list;
		va_start(args_list, format);
		vsnprintf((char *)printf_uart_device->tx_buffer, printf_uart_device->tx_buffer_length, format, args_list);
		va_end(args_list);

		printf_uart_device->tx_buffer[printf_uart_device->tx_buffer_length - 1] = '\0';
		uint32_t str_len = strlen(printf_uart_device->tx_buffer);

		printf_uart_device->TxFunc(printf_uart_device->huart, printf_uart_device->tx_buffer, str_len);
		return pdPASS;
	}
	return pdFAIL;
}

BaseType_t UD_Receive_DMA(UART_HandleTypeDef *huart, char *pData, uint16_t size)
{
	huart->ReceptionType = HAL_UART_RECEPTION_STANDARD;
	HAL_StatusTypeDef result = UART_Start_Receive_DMA(huart, (uint8_t *)pData, 1);
	if (result == HAL_OK)
	{
		return pdPASS;
	}
	else
	{
		return pdFAIL;
	}
}

BaseType_t UD_Receive_IT(UART_HandleTypeDef *huart, char *pData, uint16_t size)
{
	huart->ReceptionType = HAL_UART_RECEPTION_STANDARD;
	HAL_StatusTypeDef result = UART_Start_Receive_IT(huart, (uint8_t *)pData, size);
	if (result == HAL_OK)
	{
		return pdPASS;
	}
	else
	{
		return pdFAIL;
	}
}

BaseType_t UD_Transmit_DMA(UART_HandleTypeDef *huart, const char *pData, uint16_t size)
{
	HAL_StatusTypeDef result = HAL_UART_Transmit_DMA(huart, (uint8_t *)pData, size);
	if (result == HAL_OK)
	{
		return pdPASS;
	}
	else
	{
		return pdFAIL;
	}
}

BaseType_t UD_Transmit_IT(UART_HandleTypeDef *huart, const char *pData, uint16_t size)
{
	HAL_StatusTypeDef result = HAL_UART_Transmit_IT(huart, (uint8_t *)pData, size);
	if (result == HAL_OK)
	{
		return pdPASS;
	}
	else
	{
		return pdFAIL;
	}
}

BaseType_t UD_Transmit(UART_HandleTypeDef *huart, const char *pData, uint16_t size, uint32_t timeout)
{

	HAL_StatusTypeDef result = HAL_UART_Transmit(huart, (uint8_t *)pData, size, timeout);
	if (result == HAL_OK)
	{
		return pdPASS;
	}
	else
	{
		return pdFAIL;
	}
}

BaseType_t UD_Open(UART_DEVICE *uart_device)
{
	if (uart_device == NULL)
		return pdFAIL;

	xSemaphoreGive(uart_device->tx_sem);
	uart_device->RxFunc(uart_device->huart, &uart_device->rx_temp_char, 1);
	uart_device->is_open = pdTRUE;
	return pdPASS;
}

void UD_Close(UART_DEVICE *uart_device)
{
	if (uart_device == NULL)
		return;
	xSemaphoreTake(uart_device->tx_sem, portMAX_DELAY);
	uart_device->is_open = pdFALSE;
}

UART_DEVICE *UD_Find(UART_HandleTypeDef *huart)
{
	if (huart == NULL)
		return NULL;

	for (int i = 0; i < MAX_UartDevice_Num; i++)
	{
		if (uart_devices[i]->huart->Instance == huart->Instance)
		{
			return uart_devices[i];
		}
	}

	return NULL;
}

UART_DEVICE *UD_New(UART_HandleTypeDef *huart, uint16_t tx_buffer_length, uint16_t rx_queue_length, UartDevice_Mode tx_mode, UartDevice_Mode rx_mode)
{
	if (huart == NULL)
		return NULL;

	for (int i = 0; i < MAX_UartDevice_Num; i++)
	{
		if (uart_devices[i] == NULL)
		{
			// 设备结构体
			uart_devices[i] = pvPortMalloc(sizeof(UART_DEVICE));
			if (uart_devices[i] == NULL) // 内存不足
			{
				return NULL;
			}

			memset(uart_devices[i], 0, sizeof(*uart_devices[i])); // 清零结构体

			// 发送缓冲区
			uart_devices[i]->tx_buffer_length = tx_buffer_length;
			if (uart_devices[i]->tx_buffer_length > 0) // 使用发送缓冲区
			{
				uart_devices[i]->tx_buffer = pvPortMalloc(tx_buffer_length);
				if (uart_devices[i]->tx_buffer == NULL) // 内存不足
				{
					// 删除创建了一半的设备
					UD_Del(uart_devices[i]);
					return NULL;
				}
			}
			else // 不使用发送缓冲区
			{
				uart_devices[i]->tx_buffer_length = 0;
				uart_devices[i]->tx_buffer = NULL;
			}

			// 发送信号量
			uart_devices[i]->tx_sem = xSemaphoreCreateBinary();
			if (uart_devices[i]->tx_sem == NULL) // 内存不足
			{
				// 删除创建了一半的设备
				UD_Del(uart_devices[i]);
				return NULL;
			}

			// 接收消息队列
			uart_devices[i]->rx_queue_length = rx_queue_length;
			if (uart_devices[i]->rx_queue_length > 0) // 使用接收消息队列
			{
				uart_devices[i]->rx_queue = xQueueCreate(uart_devices[i]->rx_queue_length, (unsigned portBASE_TYPE)sizeof(char));
				if (uart_devices[i]->rx_queue == NULL) // 内存不足
				{
					// 删除创建了一半的设备
					UD_Del(uart_devices[i]);
					return NULL;
				}
			}
			else // 不使用接收消息队列
			{
				uart_devices[i]->rx_queue_length = 0;
				uart_devices[i]->rx_queue = NULL;
			}

			uart_devices[i]->is_open = pdFALSE;
			uart_devices[i]->huart = huart;

			switch (tx_mode)
			{
			case UartDevice_DMA:
				uart_devices[i]->TxFunc = UD_Transmit_DMA;
				uart_devices[i]->TxFuncBlock = UD_Transmit;
				break;
			case UartDevice_IT:
				uart_devices[i]->TxFunc = UD_Transmit_IT;
				uart_devices[i]->TxFuncBlock = UD_Transmit;
				break;
			default:
				uart_devices[i]->TxFunc = UD_Transmit_IT;
				uart_devices[i]->TxFuncBlock = UD_Transmit;
				break;
			}

			switch (rx_mode)
			{
			case UartDevice_DMA:
				uart_devices[i]->RxFunc = UD_Receive_DMA;
				break;
			case UartDevice_IT:
				uart_devices[i]->RxFunc = UD_Receive_IT;
				break;
			default:
				uart_devices[i]->RxFunc = UD_Receive_IT;
				break;
			}

			return uart_devices[i];
		}
	}

	return NULL;
}

void UD_Del(UART_DEVICE *uart_device)
{
	if (uart_device == NULL)
		return;

	uart_device->is_open = pdFALSE;
	uart_device->huart = NULL;

	// 发送相关
	uart_device->tx_buffer_length = 0;
	uart_device->RxFunc = NULL;
	if (uart_device->tx_buffer != NULL)
	{
		vPortFree(uart_device->tx_buffer);
		uart_device->tx_buffer = NULL;
	}

	if (uart_device->tx_sem != NULL)
	{
		vSemaphoreDelete(uart_device->tx_sem);
		uart_device->tx_sem = NULL;
	}

	// 接收相关
	uart_device->rx_queue_length = 0;
	uart_device->rx_temp_char = 0;
	uart_device->TxFunc = NULL;
	uart_device->TxFuncBlock = NULL;
	if (uart_device->rx_queue != NULL)
	{
		vQueueDelete(uart_device->rx_queue);
		uart_device->rx_queue = NULL;
	}

	// 本体
	for (int i = 0; i < MAX_UartDevice_Num; i++)
	{
		if (uart_devices[i] == uart_device)
		{
			vPortFree(uart_devices[i]);
			uart_devices[i] = NULL;
		}
	}
	uart_device = NULL;
}

BaseType_t UD_WriteStr(UART_DEVICE *uart_device, const char *str, uint16_t length, uint32_t timeout)
{
	if (uart_device == NULL || uart_device->is_open == pdFALSE)
		return pdFAIL;

	if (xSemaphoreTake(uart_device->tx_sem, timeout) == pdPASS)
	{
		uart_device->TxFunc(uart_device->huart, str, length);
		return pdPASS;
	}

	return pdFAIL;
}

BaseType_t UD_WriteStrCopy(UART_DEVICE *uart_device, const char *str, uint16_t length, uint32_t timeout)
{
	if (uart_device == NULL || uart_device->is_open == pdFALSE)
		return pdFAIL;

	if (xSemaphoreTake(uart_device->tx_sem, timeout) == pdPASS)
	{
		uint32_t str_len = length;
		if (str_len > uart_device->tx_buffer_length)
			str_len = uart_device->tx_buffer_length;

		memcpy(uart_device->tx_buffer, str, str_len);
		uart_device->TxFunc(uart_device->huart, uart_device->tx_buffer, str_len);
		return pdPASS;
	}

	return pdFAIL;
}

BaseType_t UD_WriteChar(UART_DEVICE *uart_device, char cha, uint32_t timeout)
{
	if (uart_device == NULL || uart_device->is_open == pdFALSE)
		return pdFAIL;

	if (xSemaphoreTake(uart_device->tx_sem, timeout) == pdPASS)
	{
		BaseType_t result;
		result = uart_device->TxFuncBlock(uart_device->huart, &cha, 1, timeout);
		xSemaphoreGive(uart_device->tx_sem);
		return result;
	}

	return pdFAIL;
}

void UD_Sync(UART_DEVICE *uart_device)
{
	if (uart_device == NULL || uart_device->is_open == pdFALSE)
		return;
	xSemaphoreTake(uart_device->tx_sem, portMAX_DELAY);
	xSemaphoreGive(uart_device->tx_sem);
}

BaseType_t UD_Read(UART_DEVICE *uart_device, void *const read_buffer, uint32_t timeout)
{
	if (uart_device == NULL || uart_device->is_open == pdFALSE)
		return pdFAIL;

	return xQueueReceive(uart_device->rx_queue, read_buffer, timeout);
}

void UD_TxCpltCallback(UART_HandleTypeDef *huart)
{
	UART_DEVICE *uart_device = UD_Find(huart);

	if (uart_device != NULL && uart_device->is_open != pdFALSE)
	{
		BaseType_t xHigherPriorityTaskWoken = pdFALSE;
		xSemaphoreGiveFromISR(uart_device->tx_sem, &xHigherPriorityTaskWoken);
		portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
	}
}

void UD_RxCpltCallback(UART_HandleTypeDef *huart)
{
	UART_DEVICE *uart_device = UD_Find(huart);

	if (uart_device != NULL && uart_device->is_open != pdFALSE)
	{
		BaseType_t xHigherPriorityTaskWoken = pdFALSE;
		xQueueSendFromISR(uart_device->rx_queue, (uint8_t *)&uart_device->rx_temp_char, &xHigherPriorityTaskWoken);
		uart_device->RxFunc(uart_device->huart, &uart_device->rx_temp_char, 1);
		portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
	}
}

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
	UD_TxCpltCallback(huart);
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	UD_RxCpltCallback(huart);
}
