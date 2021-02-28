#include <od/objects/mixing/StereoCrossFade.h>
#include <od/config.h>
#include <hal/simd.h>

namespace od
{

  StereoCrossFade::StereoCrossFade()
  {
    addInput(mLeftInputA);
    addInput(mLeftInputB);
    addInput(mRightInputA);
    addInput(mRightInputB);
    addInput(mFade);
    addOutput(mLeftOutput);
    addOutput(mRightOutput);
  }

  StereoCrossFade::~StereoCrossFade()
  {
  }

  void StereoCrossFade::process()
  {
    float *leftA = mLeftInputA.buffer();
    float *leftB = mLeftInputB.buffer();
    float *rightA = mRightInputA.buffer();
    float *rightB = mRightInputB.buffer();
    float *fade = mFade.buffer();
    float *leftOut = mLeftOutput.buffer();
    float *rightOut = mRightOutput.buffer();
    float *end = fade + globalConfig.frameLength;

    float32x4_t zero = vdupq_n_f32(0.0f);
    float32x4_t one = vdupq_n_f32(1.0f);
    float32x4_t two = vdupq_n_f32(2.0f);
    float32x4_t x, x2, w1, w2, a, b;

    while (fade < end)
    {
      x = vld1q_f32(fade);
      x = vminq_f32(x, one);
      x = vmaxq_f32(x, zero);
      x2 = x * x;
      w1 = two * x - x2;
      w2 = one - x2;

      a = vld1q_f32(leftA);
      b = vld1q_f32(leftB);
      x = w1 * a + w2 * b;
      vst1q_f32(leftOut, x);

      a = vld1q_f32(rightA);
      b = vld1q_f32(rightB);
      x = w1 * a + w2 * b;
      vst1q_f32(rightOut, x);

      fade += 4;
      leftOut += 4;
      rightOut += 4;
      leftA += 4;
      leftB += 4;
      rightA += 4;
      rightB += 4;
    }
  }

} /* namespace od */
