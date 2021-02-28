#include <od/audio/MonoResampler.h>
#include <hal/simd.h>
#include <od/config.h>

namespace od
{

  MonoResampler::MonoResampler()
  {
  }

  MonoResampler::~MonoResampler()
  {
  }

  inline float *MonoResampler::incrementPhase(float *in)
  {
    mPhase += mPhaseDelta;
    int k = (int)mPhase;
    mPhase -= k;

    if (k > 3)
    {
      in += k - 3;
      k = 3;
    }

    // going forwards
    while (k > 0)
    {
      k--;
      // consume next sample
      mFifo[2] = mFifo[1];
      mFifo[1] = mFifo[0];
      mFifo[0] = in[0];
      in++;
    }

    return in;
  }

  int MonoResampler::nextFrame(float *in, float *out)
  {
    float *orig = in;
    float *end = out + FRAMELENGTH;
    while (out < end)
    {
      in = incrementPhase(in);
      mNeonPhase[0] = mPhase;
      mNeonRecent0[0] = mFifo[0];
      mNeonRecent1[0] = mFifo[1];
      mNeonRecent2[0] = mFifo[2];

      in = incrementPhase(in);
      mNeonPhase[1] = mPhase;
      mNeonRecent0[1] = mFifo[0];
      mNeonRecent1[1] = mFifo[1];
      mNeonRecent2[1] = mFifo[2];

      in = incrementPhase(in);
      mNeonPhase[2] = mPhase;
      mNeonRecent0[2] = mFifo[0];
      mNeonRecent1[2] = mFifo[1];
      mNeonRecent2[2] = mFifo[2];

      in = incrementPhase(in);
      mNeonPhase[3] = mPhase;
      mNeonRecent0[3] = mFifo[0];
      mNeonRecent1[3] = mFifo[1];
      mNeonRecent2[3] = mFifo[2];

      simd_quadratic_interpolate(out, mNeonRecent0, mNeonRecent1, mNeonRecent2,
                                 mNeonPhase);
      out += 4;
    }

    return in - orig;
  }

} /* namespace od */
