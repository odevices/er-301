#include <core/objects/SnapToZero.h>
#include <math.h>
#include <od/config.h>
#include <hal/simd.h>

namespace od
{

  SnapToZero::SnapToZero()
  {
    addInput(mInput);
    addOutput(mOutput);
  }

  SnapToZero::~SnapToZero()
  {
  }

  void SnapToZero::setThreshold(float threshold)
  {
    mThreshold = threshold;
  }

  void SnapToZero::setThresholdInDecibels(float decibels)
  {
    mThreshold = powf(10.0f, decibels * (1.0f / 20.0f));
  }

  void SnapToZero::process()
  {
    float *in = mInput.buffer();
    float *out = mOutput.buffer();
    float32x4_t zero = vdupq_n_f32(0);
    float32x4_t threshold = vdupq_n_f32(mThreshold);

    for (int i = 0; i < FRAMELENGTH; i += 4)
    {
      float32x4_t x = vld1q_f32(in + i);
      float32x4_t ax = vabsq_f32(x);
      uint32x4_t under = vcltq_f32(ax, threshold);
      vst1q_f32(out + i, vbslq_f32(under, zero, x));
    }
  }

} /* namespace od */
