/**
 * @file PID.H
 * @author TITH (1023515576@qq.com)
 * @brief PID
 * @version 0.1
 * @date 2022-07-19
 *
 * @copyright Copyright (c) 2022
 *
 */

#pragma once

typedef struct
{
    float KP;        // PID参数P
    float KI;        // PID参数I
    float KD;        // PID参数D
    float fdb;       // PID反馈值
    float ref;       // PID目标值
    float cur_error; //当前误差
    float error[2];  //前两次误差
    float output;    //输出值
    float outputMax; //最大输出值的绝对值
    float outputMin; //最小输出值的绝对值用于防抖
} GenPID_t;

void GenPID_Servo(float ref, float fdb, GenPID_t *pid);
