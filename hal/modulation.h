#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

  void Modulation_init();
  void Modulation_start(void);
  void Modulation_restart(void);

  void Modulation_setChannelRange(uint32_t channel, uint32_t range);
  uint32_t Modulation_getChannelRange(uint32_t channel);

  // Called on each frame in their respective threads.
  // There are 2 because the ER-301 has 2 ADC ICs each on a separate SPI bus.
  extern void Mod0_callback(int *samples);
  extern void Mod1_callback(int *samples);

#ifdef __cplusplus
}
#endif
