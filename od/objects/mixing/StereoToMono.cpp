#include <od/objects/mixing/StereoToMono.h>
#include <od/config.h>
#include <hal/simd.h>
#include <string.h>

namespace od
{

  StereoToMono::StereoToMono()
  {
    addInput(mLeftInput);
    addInput(mRightInput);
    addOutput(mOutput);
    addOption(mRouting);
  }

  StereoToMono::~StereoToMono()
  {
  }

  void StereoToMono::process()
  {
    float *out = mOutput.buffer();

    if (mRouting.value() == CHOICE_BOTH)
    {
      float *left = mLeftInput.buffer();
      float *right = mRightInput.buffer();
      float *end = out + globalConfig.frameLength;

      float32x4_t y, x0, x1, g = vdupq_n_f32(0.75f);

      while (out < end)
      {
        x0 = vld1q_f32(left);
        x1 = vld1q_f32(right);
        y = vmulq_f32(vaddq_f32(x0, x1), g);
        vst1q_f32(out, y);
        out += 4;
        left += 4;
        right += 4;
      }
    }
    else if (mRouting.value() == CHOICE_LEFT)
    {
      float *left = mLeftInput.buffer();
      memcpy(out, left, FRAMELENGTH * sizeof(float));
    }
    else if (mRouting.value() == CHOICE_RIGHT)
    {
      float *right = mRightInput.buffer();
      memcpy(out, right, FRAMELENGTH * sizeof(float));
    }
  }

} /* namespace od */
