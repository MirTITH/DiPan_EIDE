/**
 * @file CLI_startup.c
 * @author TITH (1023515576@qq.com)
 * @brief CLI控制台启动代码，若要添加自定义命令，请在CLI_custom_command.c中添加
 * @version 2.0
 * @date 2022-01-13
 *
 * @copyright Copyright (c) 2022
 *
 */
#include "uart_device.h"
#include "CLI.h"

UART_HandleTypeDef *CLI_huart;
UART_DEVICE *cli_uart_device;
osThreadId cli_taskHandle;

extern void cli_task(void const *argument);

void CLI_Init(UART_HandleTypeDef *huart)
{
	/* creation of cli */
	CLI_huart = huart;
	cli_uart_device = UartDevice_New(CLI_huart, 0, 64, UartDevice_DMA, UartDevice_IT);
	UartDevice_Open(cli_uart_device);
	osThreadDef(CLI, cli_task, osPriorityBelowNormal, 0, 128);
	cli_taskHandle = osThreadCreate(osThread(CLI), NULL);
}

#if (configGENERATE_RUN_TIME_STATS == 1)
unsigned long getRunTimeCounterValue(void)
{
	return xTaskGetTickCount(); //返回当前滴答数
}
#endif
