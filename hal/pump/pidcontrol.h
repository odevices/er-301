#pragma once

#ifdef __cplusplus
extern "C"
{
#endif

  typedef struct
  {
    float Kp, Ti, Td; // error gain, integration time, derivative time
    float A, B, C;
    float e[3];
    float SP; // set point
    float MV; // manipulated variable
    float upper, lower;
  } PIDController;

  void PIDController_init(PIDController *pid, float SP, float Kp, float Ti,
                          float Td, float dt, float upper, float lower);

  float PIDController_update(PIDController *pid, float PV);

  typedef struct
  {
    float Kp, Ti;
    float A;
    float e[2];
    float SP;
    float MV;
    float upper, lower;
  } PDController;

  void PDController_init(PDController *pid, float SP, float Kp, float Ti,
                         float dt, float upper, float lower);

  float PDController_update(PDController *pid, float PV);

  typedef struct
  {
    float Kp, Ki;
    float dt;
    float integral;
    float SP;
    float MV;
    float upper, lower;
  } PIController;

  void PIController_init(PIController *pid, float SP, float Kp, float Ti,
                         float dt, float upper, float lower);

  float PIController_update(PIController *pid, float PV);

#ifdef __cplusplus
}
#endif
