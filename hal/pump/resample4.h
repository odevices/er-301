#pragma once

#include "rfifo4.h"

#ifdef __cplusplus
extern "C"
{
#endif

  typedef struct
  {
    rfifo4_t fifo;
    float pos;
    uint32_t underflow;
    uint32_t overflow;
  } resampler4_t;

  static inline void resampler4_reset_diagnostics(resampler4_t *resampler)
  {
    resampler->underflow = 0;
    resampler->overflow = 0;
  }

  static inline void resampler4_init(resampler4_t *resampler, uint32_t n)
  {
    rfifo4_alloc(&resampler->fifo, n);
    // half-fill
    //for(int i=0;i<n;i+=2) rfifo4_push1(&resampler->fifo,vdupq_n_f32(0.0f));
    resampler->pos = 0.0;
    resampler4_reset_diagnostics(resampler);
  }

  static inline unsigned int resampler4_count(resampler4_t *resampler)
  {
    return rfifo4_count(&resampler->fifo);
  }

  static inline void resampler4_write(resampler4_t *resampler, float32x4_t *samples, uint32_t n)
  {
    rfifo4_push(&resampler->fifo, samples, n);
  }

  static inline void resampler4_write1(resampler4_t *resampler, float32x4_t sample)
  {
    rfifo4_push1(&resampler->fifo, sample);
  }

  // ratio < 1.0: Consume ratio input samples for every output sample.
  void resampler4_upSample(resampler4_t *resampler, float32x4_t *out, int n, int stride, float ratio);

  // ratio > 1.0: Consume ratio input samples for every output sample.
  void resampler4_downSample(resampler4_t *resampler, float32x4_t *out, int n, int stride, float ratio);

#ifdef __cplusplus
}
#endif
