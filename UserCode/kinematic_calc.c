/**
 * @file kinematic_calc.c
 * @author TITH (1023515576@qq.com)
 * @brief 运动学解算
 * @version 0.1
 * @date 2022-03-21
 *
 * @copyright Copyright (c) 2022
 *
 */

#include "kinematic_calc.h"
#include <math.h>

const double PI = 3.141592653589793238462643383279;

typedef struct WHEEL_CTRL
{
	double forward_v;	// 前进轮期望速度 (m/s)
	double forward_pos; // 前进轮期望位置 (m)
	double rot_v;		// 旋转期望角速度 (rad/s)
	double rot_pos;		// 旋转期望位置 (与 x 轴平行为0，逆时针为正，一圈是 2 PI)
} Wheel_Ctrl;

/**
 *
 * 舵轮对应位置
 * 0  1
 * 2  3
 */
typedef struct WHEEL
{
	double o_pos_x; // 舵轮相对控制中心的 x 位置 (m)
	double o_pos_y; // 舵轮相对控制中心的 y 位置 (m)
	Wheel_Ctrl wheel_exp;
	Wheel_Ctrl wheel_now;
} Wheel;

Wheel wheel[Wheel_Num] = {0};

void Kine_Read_All_Wheel_Data(void)
{
	for (int i = 0; i < Wheel_Num; i++)
	{
		// TO DO

		// wheel[i].wheel_now.forward_pos =
		// wheel[i].wheel_now.forward_v =
		// wheel[i].wheel_now.rot_pos =
		// wheel[i].wheel_now.rot_v =
	}
}

void Kine_Init(double distance_x, double distance_y, double o_offset_x, double o_offset_y)
{
	// 计算各舵轮相对控制中心的位置
	wheel[0].o_pos_x = -distance_x / 2 - o_offset_x;
	wheel[0].o_pos_y = distance_y / 2 - o_offset_y;
	wheel[1].o_pos_x = distance_x / 2 - o_offset_x;
	wheel[1].o_pos_y = distance_y / 2 - o_offset_y;
	wheel[2].o_pos_x = -distance_x / 2 - o_offset_x;
	wheel[2].o_pos_y = -distance_y / 2 - o_offset_y;
	wheel[3].o_pos_x = distance_x / 2 - o_offset_x;
	wheel[3].o_pos_y = -distance_y / 2 - o_offset_y;

	Kine_Read_All_Wheel_Data();
}
/**
 * @brief 计算舵轮的速度和角度
 * 
 * @param wheel 
 * @param wheel_vx 舵轮平动 x 方向速度
 * @param wheel_vy 舵轮平动 y 方向速度
 */
void Kine_Wheel_Calc(Wheel *wheel, double wheel_vx, double wheel_vy)
{
	if (wheel_vx == 0 && wheel_vy == 0)
	{
		wheel->wheel_exp.forward_v = 0;
	}
	else
	{
		double speed = sqrt(pow(wheel_vx, 2) + pow(wheel_vy, 2)); // 速度
		double angle = atan(wheel_vy / wheel_vx);				  // 角度，弧度制

		// 角度映射到 [0, 2*PI)
		if (wheel_vx >= 0 && wheel_vy >= 0)
		{
			angle = angle;
		}
		else if (wheel_vx < 0 && wheel_vy >= 0)
		{
			angle = PI + angle;
		}
		else if (wheel_vx < 0 && wheel_vy < 0)
		{
			angle = PI + angle;
		}
		else
		{
			angle = 2 * PI + angle;
		}

		// 旋转方向判定
		double angle_diff = fmod(angle - wheel->wheel_now.rot_pos, 2 * PI);

		if (angle_diff <= -1.5 * PI)
		{
			angle_diff += 2 * PI;
		}
		else if (angle_diff <= -0.5 * PI)
		{
			angle_diff += PI;
			speed = -speed;
		}
		else if (angle_diff <= 0.5 * PI)
		{
			// 不用转换
		}
		else if (angle_diff <= 1.5 * PI)
		{
			angle_diff -= PI;
			speed = -speed;
		}
		else
		{
			angle_diff -= 2 * PI;
		}

		// 最终写入
		wheel->wheel_exp.forward_v = speed;
		wheel->wheel_exp.forward_pos += angle_diff;
	}
}

void Kine_SetSpeed(double robot_vx, double robot_vy, double robot_rot)
{
	double wheel_vx, wheel_vy;
	for (int i = 0; i < Wheel_Num; i++)
	{
		wheel_vx = robot_vx - wheel[i].o_pos_y * robot_rot;
		wheel_vy = robot_vy + wheel[i].o_pos_x * robot_rot;
		Kine_Wheel_Calc(&wheel[i], wheel_vx, wheel_vy);
	}
}
