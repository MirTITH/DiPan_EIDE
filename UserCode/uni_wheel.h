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

typedef struct
{
	double exp_rot_pos;
	double exp_speed;
	double now_rot_pos;
}Uni_Wheel_t;

void Wheel_SetXY(Uni_Wheel_t *wheel, double speed_x, double speed_y);
