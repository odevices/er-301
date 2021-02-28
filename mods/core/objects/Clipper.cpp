#include <core/objects/Clipper.h>
#include <od/config.h>
#include <hal/simd.h>

namespace od
{

  Clipper::Clipper()
  {
    addInput(mInput);
    addOutput(mOutput);
  }

  Clipper::Clipper(float min, float max) : mMinimum(min), mMaximum(max)
  {
    addInput(mInput);
    addOutput(mOutput);
  }

  Clipper::~Clipper()
  {
  }

#if 1

  // 600 ticks for 128 samples
  void Clipper::process()
  {
    float *in = mInput.buffer();
    float *out = mOutput.buffer();
    float32x4_t max = vdupq_n_f32(mMaximum);
    float32x4_t min = vdupq_n_f32(mMinimum);
    float32x4_t x;

    for (int i = 0; i < FRAMELENGTH; i += 4)
    {
      x = vld1q_f32(in + i);
      x = vminq_f32(x, max);
      x = vmaxq_f32(x, min);
      vst1q_f32(out + i, x);
    }
  }

#else

  // 9600 ticks for 128 samples
  void Clipper::process()
  {
    float *in = mInput.buffer();
    float *out = mOutput.buffer();
    float *end = out + globalConfig.frameLength;

    while (out < end)
    {
      if (*in > mMaximum)
      {
        *(out++) = mMaximum;
      }
      else if (*in < mMinimum)
      {
        *(out++) = mMinimum;
      }
      else
      {
        *(out++) = *(in);
      }
      in++;
    }
  }

#endif

} // namespace od
/* namespace od */
