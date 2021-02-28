#pragma once

#ifdef __cplusplus
extern "C"
{
#endif

#define PWM_NUM_CHANNELS 12

  void Pwm_init(void);
  void Pwm_start(void);
  void Pwm_set(int channel, float value);
  void Pwm_set_raw(int channel, float red, float green);

#ifdef __cplusplus
}
#endif
