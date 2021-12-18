#pragma once

#ifdef __cplusplus
extern "C"
{
#endif

#ifdef __x86_64__
  #define USE_SSE4
  #include <hal/neon2sse.h>
#else
  #include <arm_neon.h>
#endif

#ifdef __cplusplus
}
#endif
