/**
 * @file uni_wheel.h
 * @author TITH (1023515576@qq.com)
 * @brief 舵轮驱动
 * @version 0.1
 * @date 2022-07-07
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#pragma once

#include "DJI.h"
#include "vesc.h"

typedef struct
{
	double loc_x; // 舵轮在机器人坐标系中的位置
	double loc_y; // 舵轮在机器人坐标系中的位置

	double exp_rot_pos;
	double exp_speed;
	double now_rot_pos;

	double rot_pos_offset; // 角位置偏置，(rot_pos + rot_pos_offset) * rot_pos_ratio = PID 的值
	double rot_pos_ratio;  // 角位置比率，(rot_pos + rot_pos_offset) * rot_pos_ratio = PID 的值
	double speed_ratio;    // 速度比率，speed * speed_ratio = ERPM
	uint16_t hall_GPIO_PINx;
	DJI_t *hDJI;
	VESC_t hvesc;
}uni_wheel_t;

void Wheels_Init(uni_wheel_t *wheel);

void Wheel_Set(uni_wheel_t *wheel, double speed, double rot_pos);

void Wheel_SetXY(uni_wheel_t *wheel, double speed_x, double speed_y);

void Wheels_CalcTransmit(uni_wheel_t wheel[], int num);

void Chassis_SetOrigin(uni_wheel_t *wheel, double origin_x, double origin_y);