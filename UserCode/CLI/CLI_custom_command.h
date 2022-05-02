/**
 * @file CLI_custom_command.h
 * @author TITH (1023515576@qq.com)
 * @brief 自定义命令的头文件，不需要在外部包含
 * @version 1.5
 * @date 2022-01-18
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#pragma once

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "FreeRTOS.h"
#include "task.h"
#include "FreeRTOS_CLI.h"
#include "cmsis_os.h"
#include "CLI_config.h"

/**
 * @brief 注册新命令
 * @param command_name 命令的名字
 * @param help_str 会显示在help命令中的帮助信息
 * @param func 命令对应的函数
 * @param param_num 命令可以接受的参数个数，-1表示可以接受任意个参数
 * 
 */
#define CLI_New_Command(command_name, help_str, func, param_num)                                                                             \
	extern BaseType_t func(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString);                                        \
	static const CLI_Command_Definition_t cd_##command_name = {#command_name, "" #command_name ": " #help_str " \r\n\r\n", func, param_num}; \
	FreeRTOS_CLIRegisterCommand(&cd_##command_name);
