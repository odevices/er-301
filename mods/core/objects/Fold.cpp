#include <core/objects/Fold.h>
#include <od/config.h>
#include <hal/simd.h>

namespace od
{

  Fold::Fold()
  {
    addInput(mInput);
    addOutput(mOutput);
    addInput(mThreshold);
    addInput(mUpperGain);
    addInput(mLowerGain);
  }

  Fold::~Fold()
  {
  }

  void Fold::process()
  {
    float *in = mInput.buffer();
    float *out = mOutput.buffer();
    float *threshold = mThreshold.buffer();
    float *upperGain = mUpperGain.buffer();
    float *lowerGain = mLowerGain.buffer();
    float *end = out + globalConfig.frameLength;

    float32x4_t zero = vdupq_n_f32(0.0f);
    float32x4_t x, lg, ug, k, upper, lower;
    while (out < end)
    {
      x = vld1q_f32(in);
      k = vld1q_f32(threshold);
      ug = vld1q_f32(upperGain);
      lg = vld1q_f32(lowerGain);

      x -= k;
      upper = vmaxq_f32(x, zero) * ug;
      lower = vminq_f32(x, zero) * lg;
      x = upper + lower;
      x += k;

      vst1q_f32(out, x);

      out += 4;
      in += 4;
      threshold += 4;
      upperGain += 4;
      lowerGain += 4;
    }
  }

} /* namespace od */
