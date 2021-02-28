#include "pidcontrol.h"

void PIDController_init(PIDController *pid, float SP, float Kp, float Ti,
												float Td, float dt, float upper, float lower)
{
	pid->SP = SP;
	pid->Kp = Kp;
	pid->Ti = Ti;
	pid->Td = Td;
	pid->upper = upper;
	pid->lower = lower;
	pid->e[0] = 0.0f;
	pid->e[1] = 0.0f;
	pid->e[2] = 0.0f;
	pid->MV = 0.0f;

	pid->A = 1.0f + dt / Ti + Td / dt;
	pid->B = -1.0f - 2.0f * Td / dt;
	pid->C = Td / dt;
}

float PIDController_update(PIDController *pid, float PV)
{
	pid->e[2] = pid->e[1];
	pid->e[1] = pid->e[0];
	pid->e[0] = pid->SP - PV;

	pid->MV += pid->Kp * (pid->A * pid->e[0] + pid->B * pid->e[1] + pid->C * pid->e[2]);

	if (pid->MV > pid->upper)
	{
		pid->MV = pid->upper;
	}
	else if (pid->MV < pid->lower)
	{
		pid->MV = pid->lower;
	}

	return pid->MV;
}

void PDController_init(PDController *pid, float SP, float Kp, float Ti,
											 float dt, float upper, float lower)
{
	pid->SP = SP;
	pid->Kp = Kp;
	pid->Ti = Ti;
	pid->upper = upper;
	pid->lower = lower;
	pid->e[0] = 0.0f;
	pid->e[1] = 0.0f;
	pid->MV = 0.0f;

	pid->A = 1.0f + dt / Ti;
}

float PDController_update(PDController *pid, float PV)
{
	pid->e[1] = pid->e[0];
	pid->e[0] = pid->SP - PV;

	pid->MV += pid->Kp * (pid->A * pid->e[0] - pid->e[1]);

	if (pid->MV > pid->upper)
	{
		pid->MV = pid->upper;
	}
	else if (pid->MV < pid->lower)
	{
		pid->MV = pid->lower;
	}

	return pid->MV;
}

void PIController_init(PIController *pid, float SP, float Kp, float Ti,
											 float dt, float upper, float lower)
{
	pid->SP = SP;
	pid->Kp = Kp;
	pid->Ki = Kp / Ti;
	pid->dt = dt;
	pid->upper = upper;
	pid->lower = lower;
	pid->MV = 0.0f;
	pid->integral = 0.0f;
}

float PIController_update(PIController *pid, float PV)
{
	float e = pid->SP - PV;
	pid->integral += e * pid->dt;

	pid->MV = pid->Kp * e + pid->Ki * pid->integral;

	if (pid->MV > pid->upper)
	{
		pid->MV = pid->upper;
	}
	else if (pid->MV < pid->lower)
	{
		pid->MV = pid->lower;
	}

	return pid->MV;
}
