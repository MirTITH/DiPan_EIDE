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
	wheel[0].o_pos_x = -(distance_x / 2 - o_offset_x);
	wheel[0].o_pos_y = distance_y / 2 - o_offset_y;
	wheel[1].o_pos_x = distance_x / 2 - o_offset_x;
	wheel[1].o_pos_y = distance_y / 2 - o_offset_y;
	wheel[2].o_pos_x = -(distance_x / 2 - o_offset_x);
	wheel[2].o_pos_y = -(distance_y / 2 - o_offset_y);
	wheel[3].o_pos_x = distance_x / 2 - o_offset_x;
	wheel[3].o_pos_y = -(distance_y / 2 - o_offset_y);

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

		// 角度映射到 [0, 2*PI)，且尽量保证角度变化最小
		if (wheel_vx > 0 && wheel_vy > 0)
		{
			angle = angle;
			if(wheel->wheel_now.rot_pos-angle>(PI/2) && wheel->wheel_now.rot_pos-angle<(3*PI/2))
			{
				angle += PI;
				speed = -speed;
			}
		}
		else if (wheel_vx < 0 && wheel_vy > 0)
		{
			angle = PI + angle;
			if(wheel->wheel_now.rot_pos-angle>(PI/2) || wheel->wheel_now.rot_pos-angle<(-PI/2))
			{
				angle += PI;
				speed = -speed;
			}
		}
		else if (wheel_vx < 0 && wheel_vy < 0)
		{
			angle = PI + angle;
			if(wheel->wheel_now.rot_pos-angle>(PI/2) || wheel->wheel_now.rot_pos-angle<(-PI/2))
			{
				angle -= PI;
				speed = -speed;
			}
		}
		else if (wheel_vx > 0 && wheel_vy < 0)
		{
			angle = 2*PI + angle;
			if(angle-wheel->wheel_now.rot_pos>(PI/2) && angle-wheel->wheel_now.rot_pos<(3*PI/2))
			{
				angle -= PI;
				speed = -speed;
			}
		}

		else if(wheel_vx == 0 && wheel_vy == 0)
		{
		  angle = wheel->wheel_now.rot_pos;
		}

		else if(wheel_vx == 0 && wheel_vy > 0)
		{
		  angle = PI/2;
		  if (wheel->wheel_now.rot_pos > PI)
		  {
			  angle += PI;
			  speed = -speed;
		  }
		}
		else if(wheel_vx == 0 && wheel_vy < 0)
		{
		  angle = 3*PI/2;
		  if (wheel->wheel_now.rot_pos < PI)
		  {
			  angle -= PI;
			  speed = -speed;
		  }
		}
		else if(wheel_vx > 0 && wheel_vy == 0)
		{
		  angle = 0;
		  if (wheel->wheel_now.rot_pos > (PI/2) && wheel->wheel_now.rot_pos < (3*PI/2))
		  {
			  angle += PI;
			  speed = -speed;
		  }
		}
		else if(wheel_vx < 0 && wheel_vy == 0)
		{
		  angle = PI;
		  if(wheel->wheel_now.rot_pos < (PI/2) || wheel->wheel_now.rot_pos > (3*PI/2))
		  {
			  angle -= PI;
			  speed = -speed;
		  }
		}
		

		// 最终写入
		wheel->wheel_exp.forward_v = speed;
		wheel->wheel_exp.rot_pos = angle;
	}
}

void Kine_SetSpeed(double robot_vx, double robot_vy, double robot_rot)
{
	double wheel_vx, wheel_vy;
	for (int i = 0; i < 4; i++)
	{
		wheel_vx = robot_vx - wheel[i].o_pos_y * robot_rot;
		wheel_vy = robot_vy + wheel[i].o_pos_x * robot_rot;
		Kine_Wheel_Calc(&wheel[i], wheel_vx, wheel_vy);
	}
}