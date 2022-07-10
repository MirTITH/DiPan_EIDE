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
	double speed_ratio;	   // 速度比率，speed * speed_ratio = ERPM
	double hall_on_pos;	   // 霍尔检测到时的 now_rot_pos
	double hall_off_pos;   // 霍尔失去时的 now_rot_pos
	double hall_angle;	   // 霍尔传感器所在的角度
	uint16_t hall_GPIO_PINx;
	DJI_t *hDJI;
	VESC_t hvesc;
}uni_wheel_t;

void Chassis_Init(uni_wheel_t *wheel);

void Wheel_Set(uni_wheel_t *wheel, double speed, double rot_pos);

void Wheel_SetXY(uni_wheel_t *wheel, double speed_x, double speed_y);

void Wheels_CalcTransmit(uni_wheel_t wheel[], int num);

void Chassis_SetOrigin(uni_wheel_t *wheel, double origin_x, double origin_y);

void Chassis_SetSpeed(uni_wheel_t *wheel, int num, double vx, double vy, double ang_v);

void Wheel_Hall_Callback(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin, uni_wheel_t *wheel);
