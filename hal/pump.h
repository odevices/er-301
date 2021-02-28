#pragma once

#ifdef __cplusplus
extern "C"
{
#endif

  void Pump_init();
  void Pump_resetThrottle();

  // Called on each frame in the audio thread.
  extern void Pump_callback(float *inputs, float *outputs);



#ifdef __cplusplus
}
#endif
