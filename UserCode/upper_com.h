/**
 * @file upper_com.h
 * @author TITH (1023515576@qq.com)
 * @brief
 * @version 0.1
 * @date 2022-07-16
 *
 * @copyright Copyright (c) 2022
 *
 */
#pragma once

#include "wtr_mavlink.h"

void UpperComTaskInit();
void UpperComTaskStart(mavlink_controller_t *ctrl_data);