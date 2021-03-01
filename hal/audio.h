#pragma once

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C"
{
#endif

// Audio configuration
#define AUDIO_NUM_CHANNELS 4
#define AUDIO_MAX_OUTPUT_VALUE ((1 << 23) - 1)
#define AUDIO_SAFE_MAX_OUTPUT_VALUE (AUDIO_MAX_OUTPUT_VALUE)

  void Audio_init();
  void Audio_start(void);
  void Audio_stop(void);
  void Audio_restart(void);
  int Audio_getRate(void);
  void Audio_printErrorStatus(void);
  int Audio_getLoad();
  bool Audio_running();
  
  // Called on each frame in the audio thread.
  extern void Audio_callback(int *samples);

#ifdef __cplusplus
}
#endif
