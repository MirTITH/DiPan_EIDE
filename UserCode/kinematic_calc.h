/**
 * @file kinematic_calc.h
 * @author TITH (1023515576@qq.com)
 * @brief 运动学解算
 * @version 0.1
 * @date 2022-03-21
 *
 * @copyright Copyright (c) 2022
 *
 */

#pragma once

#define Wheel_Num 4


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


/**
 * @brief 初始化运动学解算
 * 
 * @param distance_x 舵轮 x 方向距离，单位：米
 * @param distance_y 舵轮 y 方向距离，单位：米
 * @param o_offset_x 控制中心 x 轴偏移，单位：米
 * @param o_offset_y 控制中心 y 轴偏移，单位：米
 */
void Kine_Init(double distance_x, double distance_y, double o_offset_x, double o_offset_y);

/**
 * @brief 设置车的整体运动速度
 *
 * @param robot_vx x 轴平动速度
 * @param robot_vy y 轴平动速度
 * @param robot_rot 旋转角速度（右手系）
 */
void Kine_Wheel_Calc(Wheel *wheel, double wheel_vx, double wheel_vy);

void Kine_SetSpeed(double robot_vx, double robot_vy, double robot_rot);

void Kine_Read_All_Wheel_Data(void);

extern Wheel wheel[Wheel_Num];
extern const double PI;
