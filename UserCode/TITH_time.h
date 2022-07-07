/**
 * @file TITH_time.h
 * @author TITH (1023515576@qq.com)
 * @brief 和时间有关的一些函数
 * @version 1.0
 * @date 2022-04-16
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#pragma once
#include "main.h"

// 在 CubeMX 中设置的系统定时器
#define HAL_Tick_TIM TIM7 

/**
 * @brief 微秒级延时
 * 
 * @param us 
 */
void delay_us(uint32_t us);

/**
 * @brief 获取从芯片启动到当前的微秒数
 * 
 * @return uint32_t 
 */
uint32_t get_time_us(void);
