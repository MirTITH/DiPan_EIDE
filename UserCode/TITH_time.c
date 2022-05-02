/**
 * @file TITH_time.c
 * @author TITH (1023515576@qq.com)
 * @brief 和时间有关的一些函数
 * @version 1.0
 * @date 2022-04-16
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include "TITH_time.h"

void delay_us(uint32_t us)
{
	__IO uint32_t currentTicks = SysTick->VAL;
	/* Number of ticks per millisecond */
	const uint32_t tickPerMs = SysTick->LOAD + 1;
	/* Number of ticks to count */
	const uint32_t nbTicks = us * tickPerMs / 1000;

	/* Number of elapsed ticks */
	uint32_t elapsedTicks = 0;
	__IO uint32_t oldTicks = currentTicks;
	do
	{
		currentTicks = SysTick->VAL;
		elapsedTicks += (oldTicks < currentTicks) ? tickPerMs + oldTicks - currentTicks : oldTicks - currentTicks;
		oldTicks = currentTicks;
	} while (nbTicks > elapsedTicks);
}

uint32_t get_time_us()
{
	return HAL_GetTick() * 1000 + HAL_Tick_TIM->CNT;
}
