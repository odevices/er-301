#include <core/objects/RationalMultiply.h>
#include <od/config.h>
#include <hal/simd.h>

namespace od
{

  RationalMultiply::RationalMultiply(bool quantize) : mQuantize(quantize)
  {
    addInput(mInput);
    addInput(mNumerator);
    addInput(mDivisor);
    addOutput(mOutput);
  }

  RationalMultiply::~RationalMultiply()
  {
  }

  void RationalMultiply::process()
  {
    float *in = mInput.buffer();
    float *num = mNumerator.buffer();
    float *div = mDivisor.buffer();
    float *out = mOutput.buffer();
    float *end = out + globalConfig.frameLength;
    float32x4_t x, nf, df;

    if (mQuantize)
    {
      int32x4_t i, one = vdupq_n_s32(1), zero = vdupq_n_s32(0);

      while (out < end)
      {
        x = vld1q_f32(in);

        nf = vld1q_f32(num);
        i = vcvtq_s32_f32(nf);
        i = vmaxq_s32(zero, i);
        nf = vcvtq_f32_s32(i);

        df = vld1q_f32(div);
        i = vcvtq_s32_f32(df);
        i = vmaxq_s32(one, i);
        df = vcvtq_f32_s32(i);
        df = simd_invert(df);

        vst1q_f32(out, x * nf * df);

        out += 4;
        in += 4;
        num += 4;
        div += 4;
      }
    }
    else
    {
      float32x4_t one = vdupq_n_f32(1), zero = vdupq_n_f32(0);

      while (out < end)
      {
        x = vld1q_f32(in);

        nf = vld1q_f32(num);
        nf = vmaxq_f32(zero, nf);

        df = vld1q_f32(div);
        df = vmaxq_f32(one, df);
        df = simd_invert(df);

        vst1q_f32(out, x * nf * df);

        out += 4;
        in += 4;
        num += 4;
        div += 4;
      }
    }
  }

} /* namespace od */
