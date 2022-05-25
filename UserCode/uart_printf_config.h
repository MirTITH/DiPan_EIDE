/**
 * @file uart_printf_config.h
 * @author TITH (1023515576@qq.com)
 * @brief printf()串口重定向配置文件，不需要include
 * @version 1.0
 * @date 2022-01-12
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#include "usart.h"

// 串口号配置
#define UART_Printf_Config_huart huart6

// 如果配置了DMA模式，可以开启以下宏定义，并使用printfDMA()输出
// 最好不要开DMA，开启后，会出现缓冲区溢出问题，原有printf()无法使用，暂时未找到解决办法
// #define UART_Printf_Config_Use_DMA
