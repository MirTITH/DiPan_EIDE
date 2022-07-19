/**
 * @file PID.c
 * @author TITH (1023515576@qq.com)
 * @brief PID，从 Calculate.c 改的，适用所有情况
 * @version 0.1
 * @date 2022-07-19
 *
 * @copyright Copyright (c) 2022
 *
 */

#include "GeneralPID.h"

#include "math.h"

//增量式PID算法
void GenPID_Calc(GenPID_t *pid)
{
	pid->cur_error = pid->ref - pid->fdb;
	pid->output += pid->KP * (pid->cur_error - pid->error[1]) + pid->KI * pid->cur_error + pid->KD * (pid->cur_error - 2 * pid->error[1] + pid->error[0]);
	pid->error[0] = pid->error[1];
	pid->error[1] = pid->ref - pid->fdb;
	/*设定输出上限*/
	if (pid->output > pid->outputMax)
		pid->output = pid->outputMax;
	if (pid->output < -pid->outputMax)
		pid->output = -pid->outputMax;
}

//比例算法
void GenPID_P_Calc(GenPID_t *pid)
{
	pid->cur_error = pid->ref - pid->fdb;
	pid->output = pid->KP * pid->cur_error;
	/*设定输出上限*/
	if (pid->output > pid->outputMax)
		pid->output = pid->outputMax;
	if (pid->output < -pid->outputMax)
		pid->output = -pid->outputMax;

	if (fabs(pid->output) < pid->outputMin)
		pid->output = 0;
}

// //位置伺服函数
// void GenPID_PosServo(float ref, float fdb, GenPID_t *pid)
// {
// 	motor->posPID.ref = ref;
// 	motor->posPID.fdb = fdb;
// 	P_Calc(&motor->posPID);

// 	motor->speedPID.ref = motor->posPID.output;
// 	motor->speedPID.fdb = motor->FdbData.rpm;
// 	PID_Calc(&motor->speedPID);
// }

void GenPID_Servo(float ref, float fdb, GenPID_t *pid)
{
	pid->ref = ref;
	pid->fdb = fdb;
	GenPID_Calc(pid);
}
