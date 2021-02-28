#include <core/objects/GridQuantizer.h>
#include <od/config.h>
#include <hal/simd.h>
#include <hal/ops.h>

namespace od
{

  GridQuantizer::GridQuantizer()
  {
    addInput(mInput);
    addOutput(mOutput);
    addParameter(mLevels);
    addParameter(mPreScale);
    addParameter(mPostScale);
  }

  GridQuantizer::~GridQuantizer()
  {
  }

  void GridQuantizer::process()
  {
    float *in = mInput.buffer();
    float *out = mOutput.buffer();
    float n = MAX(2.0f, mLevels.value());
    float32x4_t g = vdupq_n_f32(mPreScale.value() * n);
    float32x4_t invg = vdupq_n_f32(mPostScale.value() / n);
    float32x4_t x;
    int32x4_t y;

    // out = floor(12 * in)/12
    for (int i = 0; i < FRAMELENGTH; i += 4)
    {
      x = vld1q_f32(in + i);
      x = vmulq_f32(g, x);
      y = vcvtq_s32_f32(x);
      x = vcvtq_f32_s32(y);
      x = vmulq_f32(invg, x);
      vst1q_f32(out + i, x);
    }
  }

} /* namespace od */
