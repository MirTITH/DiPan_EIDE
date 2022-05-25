/**
 * @file uart_printf.c
 * @author TITH (1023515576@qq.com)
 * @brief printf()串口重定向
 * @version 1.0
 * @date 2022-01-12
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include "uart_printf.h"

/* 重定向printf */
// #ifdef __GNUC__
// 	#define PUTCHAR_PROTOTYPE  int __io_putchar(int ch)
// #else
	#define PUTCHAR_PROTOTYPE  int fputc(int ch, FILE *stream)
// #endif

#ifndef UART_Printf_Config_Use_DMA

PUTCHAR_PROTOTYPE
{
	// while (HAL_UART_Transmit(&UART_Printf_Config_huart, (uint8_t *)&ch, 1, 0xffff) == HAL_BUSY);
	return ch;
}

#else

#include "stdarg.h"
/* UART 发送缓冲 */
#define UART_DEBUG_BUFF_SIZE  500
static uint8_t UART_Debug_Buff[UART_DEBUG_BUFF_SIZE];
static uint32_t UART_Debug_BuffIndex;

PUTCHAR_PROTOTYPE
{
	UART_Debug_Buff[UART_Debug_BuffIndex] = (uint8_t) ch;
	UART_Debug_BuffIndex ++;
	return ch;
}

void printfDMA(const char *format, ...)
{
	/* 等待串口状态 */
	while(UART_Printf_Config_huart.gState != HAL_UART_STATE_READY) ;

	/* 初始缓存下标 */
	UART_Debug_BuffIndex = 0;

	/* 调用vprintf解析格式化字符串 */
	va_list args_list;
	va_start(args_list, format);
	vprintf(format, args_list);
	va_end(args_list);

	HAL_UART_Transmit_DMA(&UART_Printf_Config_huart, UART_Debug_Buff, UART_Debug_BuffIndex);
}

#endif
