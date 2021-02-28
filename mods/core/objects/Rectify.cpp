#include <core/objects/Rectify.h>
#include <od/config.h>
#include <hal/simd.h>

namespace od
{

  Rectify::Rectify()
  {
    addInput(mInput);
    addOutput(mOutput);
    addOption(mType);
  }

  Rectify::~Rectify()
  {
  }

  void Rectify::process()
  {
    float *in = mInput.buffer();
    float *out = mOutput.buffer();
    float *end = out + globalConfig.frameLength;

    switch (mType.value())
    {
    case RECTIFY_POSITIVEHALF:
    {
      float32x4_t min = vdupq_n_f32(0.0f);
      float32x4_t x;
      while (out < end)
      {
        x = vld1q_f32(in);
        x = vmaxq_f32(x, min);
        vst1q_f32(out, x);
        out += 4;
        in += 4;
      }
    }
    break;
    case RECTIFY_NEGATIVEHALF:
    {
      float32x4_t max = vdupq_n_f32(0.0f);
      float32x4_t x;
      while (out < end)
      {
        x = vld1q_f32(in);
        x = vminq_f32(x, max);
        vst1q_f32(out, x);
        out += 4;
        in += 4;
      }
    }
    break;
    case RECTIFY_FULL:
    {
      float32x4_t x;
      while (out < end)
      {
        x = vld1q_f32(in);
        x = vabsq_f32(x);
        vst1q_f32(out, x);
        out += 4;
        in += 4;
      }
    }
    break;
    }
  }

} /* namespace od */
