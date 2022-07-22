/**
 * @file controller_data_sender.h
 * @author TITH (1023515576@qq.com)
 * @brief 发送遥控器原始数据
 * @version 0.1
 * @date 2022-07-22
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#pragma once

#include "wtr_mavlink.h"
#include "main.h"
#include "cmsis_os.h"
#include "usart.h"

void CtrlDataSender_Init(UART_HandleTypeDef *huart, mavlink_channel_t chan);

void CtrlDataSender_Start(mavlink_controller_t* controller);
