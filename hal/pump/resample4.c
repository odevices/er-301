#include "resample4.h"

// Quadratic interpolation (using 3 points)
// Note: recent[2] is the most recent
static inline float32x4_t quadraticInterpolation(float32x4_t *recent,
                                                 float pos)
{
  float32x4_t p = vdupq_n_f32(pos);
  float32x4_t p2 = vdupq_n_f32(pos * pos);
  float32x4_t x = vmulq_f32(recent[2],
                            vmulq_f32(vaddq_f32(p, p2), vdupq_n_f32(0.5f)));
  x = vmlaq_f32(x, recent[1], vsubq_f32(vdupq_n_f32(1.0f), p2));
  return vmlaq_f32(x, recent[0],
                   vmulq_f32(vsubq_f32(p2, p), vdupq_n_f32(0.5f)));
}

#if 0
// ratio < 1.0: Consume ratio input samples for every output sample.
void resampler4_upSample(resampler4_t * resampler, float32x4_t * out, int n,
		int stride, float ratio) {
	uint32_t remaining;
	float32x4_t * end = out + n * stride;
	float pos = resampler->pos;
	for (; out < end; out += stride) {
		if (pos >= 1.0) {
			pos -= 1.0f;
			remaining = rfifo4_count(&resampler->fifo);
			if (remaining > 3)
				rfifo4_pop(&resampler->fifo, 1);
		} else {
			remaining = rfifo4_count(&resampler->fifo);
		}
		*out = vdupq_n_f32(remaining/(float)resampler->fifo.length);
		pos += ratio;
	}
	resampler->pos = pos;
	resampler->underflow = 0;
}
#else
// ratio < 1.0: Consume ratio input samples for every output sample.
void resampler4_upSample(resampler4_t *resampler, float32x4_t *out, int n,
                            int stride, float ratio)
{
  float32x4_t *last;
  float32x4_t *end = out + n * stride;
  float pos = resampler->pos;
  for (; out < end; out += stride)
  {
    while (pos >= 1.0)
    {
      pos -= 1.0f;
      if (rfifo4_count(&resampler->fifo) > 3)
        rfifo4_pop(&resampler->fifo, 1);
    }
    last = rfifo4_get(&resampler->fifo);
    *out = quadraticInterpolation(last, pos);
    pos += ratio;
  }
  resampler->pos = pos;
  resampler->underflow = 0;
}
#endif

// ratio > 1.0: Consume ratio input samples for every output sample.
void resampler4_downSample(resampler4_t *resampler, float32x4_t *out, int n,
                              int stride, float ratio)
{
  uint32_t remaining;
  float32x4_t *last;
  float32x4_t *end = out + n * stride;
  float pos = resampler->pos;
  for (; out < end; out += stride)
  {
    last = rfifo4_get(&resampler->fifo);
    if (pos >= 1.0)
    {
      pos -= 1.0f;
      rfifo4_pop(&resampler->fifo, 1);
      remaining = rfifo4_count(&resampler->fifo);
      if (remaining < 3)
      {
        rfifo4_push1(&resampler->fifo, last[remaining - 1]);
      }
      last = rfifo4_get(&resampler->fifo);
    }
    *out = quadraticInterpolation(last, pos);
    pos += ratio;
  }
  resampler->pos = pos;
  resampler->underflow = 0;
}
