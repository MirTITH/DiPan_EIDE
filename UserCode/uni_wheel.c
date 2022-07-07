/**
 * @file uni_wheel.c
 * @author TITH (1023515576@qq.com)
 * @brief 舵轮驱动
 * @version 0.1
 * @date 2022-07-07
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include "uni_wheel.h"
#include <math.h>
#include <stdio.h>
#include <stdbool.h>

/**
 * @brief 循环变量化简
 * 
 * @param cycle 周期
 * @param value 
 * @return double 化简后的值，在[- T / 2, T / 2] 之间
 */
double LoopSimplify(double cycle, double value)
{
	double mod_value = fmod(value , cycle);

	if (mod_value > cycle / 2)
	{
		mod_value -= cycle;
	}

	if (mod_value < -cycle / 2)
	{
		mod_value += cycle;
	}

	return mod_value;
}

/**
 * @brief 设置舵轮x,y方向的速度分量
 * 
 * @param wheel 
 * @param speed_x 
 * @param speed_y 
 */
void Wheel_SetXY(Uni_Wheel_t *wheel, double speed_x, double speed_y)
{
	double delta_pos;
	wheel->exp_speed = sqrt(speed_x * speed_x + speed_y * speed_y);
	if (wheel->exp_speed == 0)
	{
		wheel->exp_speed = wheel->now_rot_pos;
		return;
	}

	delta_pos = LoopSimplify(2 * M_PI, atan(speed_y / speed_x) - wheel->now_rot_pos);

	if (delta_pos > M_PI / 2)
	{
		delta_pos -= M_PI;
		wheel->exp_speed = -wheel->exp_speed;
	}
	else if (delta_pos < -M_PI / 2)
	{
		delta_pos += M_PI;
		wheel->exp_speed = -wheel->exp_speed;
	}

	wheel->exp_rot_pos = wheel->now_rot_pos + delta_pos;
}
