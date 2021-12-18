#pragma once

#ifdef __cplusplus
extern "C"
{
#endif

#ifdef __ARM_NEON
  #include <arm_neon.h>
#else
  #define USE_SSE4
  #include <hal/neon2sse.h>
#endif

#ifdef __cplusplus
}
#endif
