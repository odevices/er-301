#include <core/objects/Limiter.h>
#include <od/config.h>
#include <hal/simd.h>

namespace od
{

  Limiter::Limiter()
  {
    addInput(mInput);
    addOutput(mOutput);
    addOption(mType);
  }

  Limiter::~Limiter()
  {
  }

  static inline void tanhLimiting(float *in, float *out, int n)
  {
#if 0
    // tanh approximation (regular floating point)
    // https://varietyofsound.wordpress.com/2011/02/14/efficient-tanh-computation-using-lamberts-continued-fraction/
    // 55k ticks
    for(int i=0;i<n;i++)
    {
        float x = in[i];
        float x2 = x * x;
        float a = (((x2 + 378) * x2 + 17325) * x2 + 135135) * x;
        float b = ((28 * x2 + 3150) * x2 + 62370) * x2 + 135135;
        out[i] = a / b;
    }
#elif 1
    // tanh approximation (neon w/ division via newton's method)
    // https://varietyofsound.wordpress.com/2011/02/14/efficient-tanh-computation-using-lamberts-continued-fraction/
    // 10k ticks
    float32x4_t c1 = vdupq_n_f32(378);
    float32x4_t c2 = vdupq_n_f32(17325);
    float32x4_t c3 = vdupq_n_f32(135135);
    float32x4_t c4 = vdupq_n_f32(28);
    float32x4_t c5 = vdupq_n_f32(3150);
    float32x4_t c6 = vdupq_n_f32(62370);
    float32x4_t x, x2, a, b, invb;

    for (int i = 0; i < n; i += 4)
    {
      x = vld1q_f32(in + i);
      x2 = x * x;
      a = (((x2 + c1) * x2 + c2) * x2 + c3) * x;
      b = ((c4 * x2 + c5) * x2 + c6) * x2 + c3;
      // https://en.wikipedia.org/wiki/Division_algorithm#Newton.E2.80.93Raphson_division
      invb = vrecpeq_f32(b);
      // iterate 3 times for 24 bits of precision
      invb = vmulq_f32(invb, vrecpsq_f32(b, invb));
      invb = vmulq_f32(invb, vrecpsq_f32(b, invb));
      invb = vmulq_f32(invb, vrecpsq_f32(b, invb));
      vst1q_f32(out + i, a * invb);
    }
#elif 0
    // tanh approximation (neon w/ regular division)
    // https://varietyofsound.wordpress.com/2011/02/14/efficient-tanh-computation-using-lamberts-continued-fraction/
    // 28k ticks
    float32x4_t c1 = vdupq_n_f32(378);
    float32x4_t c2 = vdupq_n_f32(17325);
    float32x4_t c3 = vdupq_n_f32(135135);
    float32x4_t c4 = vdupq_n_f32(28);
    float32x4_t c5 = vdupq_n_f32(3150);
    float32x4_t c6 = vdupq_n_f32(62370);
    float32x4_t x, x2, a, b;

    for (int i = 0; i < n; i += 4)
    {
      x = vld1q_f32(in + i);
      x2 = x * x;
      a = (((x2 + c1) * x2 + c2) * x2 + c3) * x;
      b = ((c4 * x2 + c5) * x2 + c6) * x2 + c3;
      vst1q_f32(out + i, a / b);
    }
#endif
  }

  static inline void invSqrtLimiting(float *in, float *out, int n)
  {
    // x/sqrt(x*x + 1) (neon w/ inv sqrt via newton's method)
    // 7k ticks
    float32x4_t one = vdupq_n_f32(1.0f);
    float32x4_t x, b, invb;

    for (int i = 0; i < n; i += 4)
    {
      x = vld1q_f32(in + i);
      b = x * x + one;
      invb = vrsqrteq_f32(b); // 4k ticks
      // iterate 3 times for 24 bits of precision
      invb = invb * vrsqrtsq_f32(b, invb * invb); // 5k ticks
      invb = invb * vrsqrtsq_f32(b, invb * invb); //  6k ticks
      invb = invb * vrsqrtsq_f32(b, invb * invb); //  7k ticks
      vst1q_f32(out + i, x * invb);
    }
  }

  static inline void cubicLimiting(float *in, float *out, int n)
  {
    // x + c * x*x*x
    // 2200 ticks
    float32x4_t c = vdupq_n_f32(-1.0f / 6.75f);
    float32x4_t x, x3;
    float32x4_t max = vdupq_n_f32(1.5);
    float32x4_t min = vdupq_n_f32(-1.5);

    for (int i = 0; i < n; i += 4)
    {
      x = vld1q_f32(in + i);
      x = vminq_f32(x, max);
      x = vmaxq_f32(x, min);
      x3 = vmulq_f32(x, vmulq_f32(x, x));
      vst1q_f32(out + i, vmlaq_f32(x, c, x3));
    }
  }

  static inline void hardLimiting(float *in, float *out, int n)
  {
    float32x4_t max = vdupq_n_f32(1.0f);
    float32x4_t min = vdupq_n_f32(-1.0f);
    float32x4_t x;

    for (int i = 0; i < n; i += 4)
    {
      x = vld1q_f32(in + i);
      x = vminq_f32(x, max);
      x = vmaxq_f32(x, min);
      vst1q_f32(out + i, x);
    }
  }

  void Limiter::process()
  {
    float *in = mInput.buffer();
    float *out = mOutput.buffer();

    switch (mType.value())
    {
    case LIMITER_INVSQRT:
      invSqrtLimiting(in, out, FRAMELENGTH);
      break;
    case LIMITER_CUBIC:
      cubicLimiting(in, out, FRAMELENGTH);
      break;
    case LIMITER_HARD:
      hardLimiting(in, out, FRAMELENGTH);
      break;
    }
  }

} // namespace od
/* namespace od */
