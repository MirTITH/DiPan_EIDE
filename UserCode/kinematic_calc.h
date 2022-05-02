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
void Kine_SetSpeed(double robot_vx, double robot_vy, double robot_rot);

void Kine_Read_All_Wheel_Data(void);
