/**
 * @file uart_printf.h
 * @author TITH (1023515576@qq.com)
 * @brief printf()串口重定向
 * @version 1.0
 * @note 使用方法：在main.c或user_code.h中包含此文件，在MDK中启用MicroLIB
 * @date 2022-01-12
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#pragma once

#include "stdio.h"
#include "usart.h"
#include "uart_printf_config.h"

#ifdef UART_Printf_Config_Use_DMA

/**
 * @brief  DMA串口输出，用法类似printf
 * @param  format  格式化字串
 * @param  ...  可变参数
 * @retval 无
 */
void printfDMA(const char* format, ...);

#endif
