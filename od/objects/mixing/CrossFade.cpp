#include <od/objects/mixing/CrossFade.h>
#include <od/config.h>
#include <hal/simd.h>

namespace od
{

  CrossFade::CrossFade()
  {
    addInput(mInputA);
    addInput(mInputB);
    addInput(mFade);
    addOutput(mOutput);
  }

  CrossFade::~CrossFade()
  {
  }

  // 1400 ticks
  void CrossFade::process()
  {
    float *A = mInputA.buffer();
    float *B = mInputB.buffer();
    float *fade = mFade.buffer();
    float *out = mOutput.buffer();
    float *end = out + globalConfig.frameLength;

    float32x4_t zero = vdupq_n_f32(0.0f);
    float32x4_t one = vdupq_n_f32(1.0f);
    float32x4_t two = vdupq_n_f32(2.0f);
    float32x4_t x, x2, w1, w2, a, b;

    while (out < end)
    {
      x = vld1q_f32(fade);
      a = vld1q_f32(A);
      b = vld1q_f32(B);
      x = vminq_f32(x, one);
      x = vmaxq_f32(x, zero);
      x2 = x * x;
      w1 = two * x - x2;
      w2 = one - x2;
      x = w1 * a + w2 * b;
      vst1q_f32(out, x);
      fade += 4;
      out += 4;
      A += 4;
      B += 4;
    }
  }

} /* namespace od */
