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

#include "chassis.h"
#include <math.h>
#include <stdio.h>
#include <stdbool.h>
#include "useful_constant.h"
#include "Caculate.h"
#include "uart_device.h"

typedef struct
{
	double wheel_distance_x; // 两个轮子之间的距离
	double wheel_distance_y; // 两个轮子之间的距离
	double origin_offset_x; // 坐标原点的偏移量（相对于底盘中心点）
	double origin_offset_y; // 坐标原点的偏移量（相对于底盘中心点）
}Chassis_Axes_t;

Chassis_Axes_t chassis_axes = {
	.wheel_distance_x = 62,
	.wheel_distance_y = 62,
	.origin_offset_x = 0,
	.origin_offset_y = 0
};

void Wheels_Init(uni_wheel_t *wheel)
{
	wheel[0].hDJI = &hDJI[4];
	wheel[1].hDJI = &hDJI[5];
	wheel[2].hDJI = &hDJI[6];
	wheel[3].hDJI = &hDJI[7];
	
	// 霍尔还没写
	// wheel[0].hall_GPIO_PINx;
	// wheel[1].hall_GPIO_PINx;
	// wheel[2].hall_GPIO_PINx;
	// wheel[3].hall_GPIO_PINx;

	wheel[0].hvesc.hcann = &hcan1;
	wheel[0].hvesc.controller_id = 0x00;
	wheel[1].hvesc.hcann = &hcan1;
	wheel[1].hvesc.controller_id = 0x01;
	wheel[2].hvesc.hcann = &hcan1;
	wheel[2].hvesc.controller_id = 0x02;
	wheel[3].hvesc.hcann = &hcan1;
	wheel[3].hvesc.controller_id = 0x03;

	for (int i = 0; i < 4; i++)
	{
		wheel[i].rot_pos_ratio = 191;
		wheel[i].speed_ratio = 2938;
		wheel[i].rot_pos_offset = 0;
	}

	Chassis_SetOrigin(wheel, 0, 0);
}

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

void Wheel_Set(uni_wheel_t *wheel, double speed, double rot_pos)
{
	wheel->exp_speed = speed;
	wheel->exp_rot_pos = rot_pos;
}

/**
 * @brief 设置舵轮x,y方向的速度分量
 * 
 * @param wheel 
 * @param speed_x 
 * @param speed_y 
 */
void Wheel_SetXY(uni_wheel_t *wheel, double speed_x, double speed_y)
{
	wheel->now_rot_pos = (wheel->hDJI->posPID.fdb / wheel->rot_pos_ratio) - wheel->rot_pos_offset;
	double exp_speed = sqrt(speed_x * speed_x + speed_y * speed_y);
	double angle;

	if (exp_speed == 0)
	{
		Wheel_Set(wheel, exp_speed, wheel->now_rot_pos);
		return;
	}

	

	if (speed_x >= 0)
	{
		angle = atan(speed_y / speed_x);
	}
	else
	{
		angle = atan(speed_y / speed_x) + M_PI;
	}

	double delta_pos = LoopSimplify(2 * M_PI, angle - wheel->now_rot_pos);

	// if(fabs(delta_pos) > 0.1)
	// {
	// 	UD_printf("del_pos:%.2lf\n", delta_pos);
	// }

	if (delta_pos > M_PI / 2)
	{
		delta_pos -= M_PI;
		exp_speed = -exp_speed;
	}
	else if (delta_pos < -M_PI / 2)
	{
		delta_pos += M_PI;
		exp_speed = -exp_speed;
	}
	Wheel_Set(wheel, exp_speed, wheel->now_rot_pos + delta_pos);
}

void Wheels_CANTransmit(uni_wheel_t wheel[])
{
	// CanTransmit_DJI_5678(&hcan1,
	// 					 wheel[0].hDJI->speedPID.output,
	// 					 wheel[1].hDJI->speedPID.output,
	// 					 wheel[2].hDJI->speedPID.output,
	// 					 wheel[3].hDJI->speedPID.output);
	
	// for (int i = 0; i < 4; i++)
	// {
	// 	VESC_CAN_SET_ERPM(&wheel[i].hvesc, wheel[i].exp_speed * wheel[i].speed_ratio);
	// }
}

void Wheels_Calc(uni_wheel_t wheel[], int num)
{
	for (int i = 0; i < num; i++)
	{
		positionServo((wheel[i].exp_rot_pos + wheel[i].rot_pos_offset) * wheel[i].rot_pos_ratio, wheel[i].hDJI);
	}
}

void Wheels_CalcTransmit(uni_wheel_t wheel[], int num)
{	
	Wheels_Calc(wheel, num);
	Wheels_CANTransmit(wheel);
}

/**
 * @brief 设置底盘的运动
 * 
 * @param wheel 
 * @param num 轮子的数量
 * @param vx 底盘的速度 x 分量
 * @param vy 底盘的速度 y 分量
 * @param ang_v 底盘旋转角速度
 */
void Chassis_SetSpeed(uni_wheel_t *wheel, int num, double vx, double vy, double ang_v)
{

}

/**
 * @brief 设置底盘坐标系的原点
 * 
 * @param wheel 
 * @param origin_x 坐标原点相对于底盘中心点的位置
 * @param origin_y 坐标原点相对于底盘中心点的位置
 */
void Chassis_SetOrigin(uni_wheel_t *wheel, double origin_x, double origin_y)
{
	chassis_axes.origin_offset_x = origin_x;
	chassis_axes.origin_offset_y = origin_y;

	wheel[0].loc_x = -chassis_axes.wheel_distance_x / 2 - chassis_axes.origin_offset_x;
	wheel[0].loc_y = chassis_axes.wheel_distance_y / 2 - chassis_axes.origin_offset_y;

	wheel[1].loc_x = chassis_axes.wheel_distance_x / 2 - chassis_axes.origin_offset_x;
	wheel[1].loc_y = chassis_axes.wheel_distance_y / 2 - chassis_axes.origin_offset_y;

	wheel[2].loc_x = -chassis_axes.wheel_distance_x / 2 - chassis_axes.origin_offset_x;
	wheel[2].loc_y = -chassis_axes.wheel_distance_y / 2 - chassis_axes.origin_offset_y;

	wheel[3].loc_x = chassis_axes.wheel_distance_x / 2 - chassis_axes.origin_offset_x;
	wheel[3].loc_y = -chassis_axes.wheel_distance_y / 2 - chassis_axes.origin_offset_y;
}
