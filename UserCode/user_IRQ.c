/**
 * @file user_IRQ.c
 * @brief 中断回调函数都写在这里
 * 
 */

#include "main.h"
#include "uart_device.h"
#include "chassis_driver.h"
#include "wtr_mavlink.h"

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
	UD_TxCpltCallback(huart);
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	WTR_MAVLink_UART_RxCpltCallback(huart);
	UD_RxCpltCallback(huart);
}

extern uni_wheel_t wheels[4];

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	switch (GPIO_Pin)
	{
	case GPIO_PIN_9:
		Wheel_Hall_Callback(GPIOE, GPIO_Pin, &wheels[0]);
		break;
	case GPIO_PIN_10:
		Wheel_Hall_Callback(GPIOE, GPIO_Pin, &wheels[1]);
		// UWheels_Hall_Callback(1);
		break;
	case GPIO_PIN_11:
		Wheel_Hall_Callback(GPIOE, GPIO_Pin, &wheels[2]);
		// UWheels_Hall_Callback(2);
		break;
	case GPIO_PIN_12:
		Wheel_Hall_Callback(GPIOE, GPIO_Pin, &wheels[3]);
		// UWheels_Hall_Callback(3);
		break;
	default:
		// printf("EXTI %d\n", GPIO_Pin);
		break;
	}

}

extern mavlink_controller_t ControllerData;
void WTR_MAVLink_Msg_RxCpltCallback(mavlink_message_t *msg)
{
	switch (msg->msgid)
	{
	case 1:
		mavlink_msg_controller_decode(msg, &ControllerData);
		break;
	default:
		break;
	}
}
