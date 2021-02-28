#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

  void Timing_init(void);
  float wallclock(void);
  void wallclock_reset(void);
  typedef int64_t tick_t;
  tick_t ticks(void);
  float ticks2secs(tick_t nticks);
  double ticks2secsD(tick_t nticks);
  unsigned long micros(void);

#ifdef __cplusplus
}
#endif
