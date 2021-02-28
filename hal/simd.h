#pragma once

#ifdef __cplusplus
extern "C"
{
#endif

#include <hal/neon.h>
#include <stdbool.h>

  // Transcendentals
  float32x4_t simd_log(float32x4_t x);
  float32x4_t simd_exp(float32x4_t x);
  float32x4_t simd_pow(float32x4_t x, float32x4_t m);
  void simd_sincos(float32x4_t x, float32x4_t *ysin, float32x4_t *ycos);
  float32x4_t simd_sin(float32x4_t x);
  float32x4_t simd_cos(float32x4_t x);
  float32x4_t simd_invert(float32x4_t x);

  // Interpolation
  void simd_linear_interpolate(float *__restrict__ out,
                               const float *__restrict__ recent0,
                               const float *__restrict__ recent1,
                               const float *__restrict__ phase);
  void simd_quadratic_interpolate(float *__restrict__ out,
                                  const float *__restrict__ recent0,
                                  const float *__restrict__ recent1,
                                  const float *__restrict__ recent2,
                                  const float *__restrict__ phase);
  void simd_quadratic_interpolate_stereo(float *out, float *recent, float phase);
  float32x4_t simd_quadratic_interpolate_with_return(
      const float *__restrict__ recent0, const float *__restrict__ recent1,
      const float *__restrict__ recent2, const float *__restrict__ phase);
  void simd_interpolate_env_accumulate(
      float *__restrict__ out, const float *__restrict__ recent0,
      const float *__restrict__ recent1, const float *__restrict__ recent2,
      const float *__restrict__ phase, float32x4_t ep, float gain);

  // Envelopes
  float32x4_t simd_sine_env(const float *envPhase);
  float32x4_t simd_hanning(const float *envPhase);
  // |x| < 1 then 1, else 0
  float32x4_t simd_boxcar(float32x4_t x);

  // Comparisons
  bool simd_any_greater(float *in, int n, float32x4_t threshold);
  bool simd_any_positive(float *in, int n);
  int simd_first_positive(float *in, int n);
  int simd_first_positive_4x(float *in, int n);

  // Constant set
  void simd_set(float *out, int n, float value);

#ifdef __cplusplus
}
#endif
