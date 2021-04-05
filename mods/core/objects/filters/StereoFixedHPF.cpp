#include <core/objects/filters/StereoFixedHPF.h>
#include <od/config.h>
#include <math.h>

namespace od
{

  StereoFixedHPF::StereoFixedHPF()
  {
    addInput(mLeftInput);
    addInput(mRightInput);
    addOutput(mLeftOutput);
    addOutput(mRightOutput);
    addParameter(mCutoff);
  }

  StereoFixedHPF::~StereoFixedHPF()
  {
  }

  void StereoFixedHPF::process()
  {
    float *leftOut = mLeftOutput.buffer();
    float *rightOut = mRightOutput.buffer();
    float *leftIn = mLeftInput.buffer();
    float *rightIn = mRightInput.buffer();

    float32x2_t r = vdup_n_f32(1.0f - (2.0f * M_PI * mCutoff.value() * globalConfig.samplePeriod));
    float32x2_t y = mY;
    float32x2_t x1, x0 = mPrevX;

    float *end = leftOut + globalConfig.frameLength;
    while (leftOut < end)
    {
      x1 = vld1_lane_f32(leftIn++, x1, 0);
      x1 = vld1_lane_f32(rightIn++, x1, 1);
      y = vsub_f32(x1, vsub_f32(x0, vmul_f32(r, y)));
      x0 = x1;
      vst1_lane_f32(leftOut++, y, 0);
      vst1_lane_f32(rightOut++, y, 1);
    }

    mY = y;
    mPrevX = x0;
  }

} /* namespace od */
