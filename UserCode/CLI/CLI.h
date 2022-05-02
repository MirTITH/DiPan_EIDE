/**
 * @file CLI.h
 * @author TITH (1023515576@qq.com)
 * @brief CLI控制台启动代码头文件，在外部只包含本文件即可
 * @version 2.0
 * @date 2022-01-13
 * @copyright Copyright (c) 2022
 * 
 * 使用方法：
 * (1) 在CubeMX中配置串口，并使能global中断
 * (2) 打开 FreeRTOS 的 USE_TRACE_FACILITY 和 USE_STATS_FORMATTING_FUNCTIONS
 * (3) 在代码中调用 CLI_Init() 初始化 CLI 控制台
 * 
 */

#pragma once

#include "usart.h"

/**
 * @brief 初始化CLI控制台
 * 
 * @param huart 该串口需要已经初始化、使能global中断。
 */
void CLI_Init(UART_HandleTypeDef* huart);
