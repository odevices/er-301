#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

  void Rng_init(void);
  // ticks per call = 21,000
  uint64_t Rng_read64(void);
  uint32_t Rng_read32(void);

#ifdef __cplusplus
}
#endif
