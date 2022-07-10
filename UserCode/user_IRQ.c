/**
 * @file user_IRQ.c
 * @brief 中断回调函数都写在这里
 * 
 */

#include "main.h"
#include "uart_device.h"
#include "upper_part.h"
#include "uart_com.h"

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
	UD_TxCpltCallback(huart);
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	UD_RxCpltCallback(huart);
	UC_RxCpltCallback(huart);

}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	switch (GPIO_Pin)
	{
	case GPIO_PIN_9:
		// UWheels_Hall_Callback(0);
		break;
	case GPIO_PIN_10:
		// UWheels_Hall_Callback(1);
		break;
	case GPIO_PIN_11:
		// UWheels_Hall_Callback(2);
		break;
	case GPIO_PIN_12:
		// UWheels_Hall_Callback(3);
		break;
	default:
		// printf("EXTI %d\n", GPIO_Pin);
		break;
	}
}