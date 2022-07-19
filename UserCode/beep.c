/**
 * @file beep.c
 * @author TITH (1023515576@qq.com)
 * @brief 蜂鸣器
 * @version 0.1
 * @date 2022-07-19
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include "beep.h"
#include "semphr.h"
#include "gpio.h"

SemaphoreHandle_t BeepSem;

void Beep()
{
	xSemaphoreGive(BeepSem);
}

void BeepFromISR()
{
	BaseType_t xHigherPriorityTaskWoken = pdFALSE;
	xSemaphoreGiveFromISR(BeepSem, &xHigherPriorityTaskWoken);
	portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
}

void BeepTask(void const *argument)
{
	SemaphoreHandle_t *sem = (SemaphoreHandle_t *)argument;

	while (xSemaphoreTake(*sem, portMAX_DELAY))
	{
		HAL_GPIO_WritePin(BeepPinGPIOx, BeepPinGPIO_Pin, 1);
		osDelay(150);
		HAL_GPIO_WritePin(BeepPinGPIOx, BeepPinGPIO_Pin, 0);
		osDelay(100);
	}
}

void Beep_Init()
{
	BeepSem = xSemaphoreCreateCounting(100, 0);

	osThreadDef(beep_task, BeepTask, osPriorityNormal, 0, 128);
	osThreadCreate(osThread(beep_task), &BeepSem);
}