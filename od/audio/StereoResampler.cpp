/*
 * StereoResampler.cpp
 *
 *  Created on: 10 May 2017
 *      Author: clarkson
 */

#include <od/audio/StereoResampler.h>
#include <hal/simd.h>
#include <od/config.h>

namespace od
{

  StereoResampler::StereoResampler()
  {
  }

  StereoResampler::~StereoResampler()
  {
  }

  int StereoResampler::nextFrame(float *in, float *out)
  {
    float *orig = in;
    float *end = out + 2 * globalConfig.frameLength;

    while (out < end)
    {
      mPhase += mPhaseDelta;
      int k = (int)mPhase;
      mPhase -= k;

      if (k > 3)
      {
        in += 2 * (k - 3);
        k = 3;
      }

      // going forwards
      while (k > 0)
      {
        k--;
        // consume next sample
        mFifo[5] = mFifo[3];
        mFifo[4] = mFifo[2];

        mFifo[3] = mFifo[1];
        mFifo[2] = mFifo[0];

        mFifo[1] = in[1];
        mFifo[0] = in[0];

        in += 2;
      }

      simd_quadratic_interpolate_stereo(out, mFifo, mPhase);
      out += 2;
    }

    return (in - orig) / 2;
  }

  int StereoResampler::nextFrame(float *in, float *left, float *right)
  {
    float *orig = in;
    float *end = left + globalConfig.frameLength;
    float tmp[2] = {0.0f, 0.0f};

    while (left < end)
    {
      mPhase += mPhaseDelta;
      int k = (int)mPhase;
      mPhase -= k;

      if (k > 3)
      {
        in += 2 * (k - 3);
        k = 3;
      }

      // going forwards
      while (k > 0)
      {
        k--;
        // consume next sample
        mFifo[5] = mFifo[3];
        mFifo[4] = mFifo[2];

        mFifo[3] = mFifo[1];
        mFifo[2] = mFifo[0];

        mFifo[1] = in[1];
        mFifo[0] = in[0];

        in += 2;
      }

      simd_quadratic_interpolate_stereo(tmp, mFifo, mPhase);
      *left = tmp[0];
      *right = tmp[1];
      left++;
      right++;
    }

    return (in - orig) / 2;
  }

} /* namespace od */
