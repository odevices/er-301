#include <core/objects/Spread.h>
#include <hal/simd.h>
#include <od/config.h>

#define LOCAL_USE_NEON 1

namespace od
{

  Spread::Spread()
  {
    addInput(mInput);
    addInput(mSpread);
    addOutput(mLeftOutput);
    addOutput(mRightOutput);
  }

  Spread::~Spread()
  {
  }

  void Spread::process()
  {
    float *in = mInput.buffer();
    float *outL = mLeftOutput.buffer();
    float *outR = mRightOutput.buffer();
    float *spread = mSpread.buffer();

#if LOCAL_USE_NEON
    for (int i = 0; i < FRAMELENGTH; i += 4)
    {
      float32x4_t x = vld1q_f32(in + i);
      float32x4_t s = vld1q_f32(spread + i) * x;
      vst1q_f32(outL + i, x - s);
      vst1q_f32(outR + i, x + s);
    }
#else
    for (int i = 0; i < FRAMELENGTH; i++)
    {
      float s = in[i] * spread[i];
      outL[i] = in[i] - s;
      outR[i] = in[i] + s;
    }
#endif
  }

} /* namespace od */
