/*
 * RunningRMS.cpp
 *
 *  Created on: 7 Oct 2016
 *      Author: clarkson
 */

#include <od/extras/RunningRMS.h>
#include <od/config.h>
#include <hal/simd.h>
#include <math.h>

namespace od
{

  RunningRMS::RunningRMS()
  {
    mPeriod = globalConfig.frameRate * 0.05f;
  }

  RunningRMS::~RunningRMS()
  {
    // TODO Auto-generated destructor stub
  }

  void RunningRMS::setPeriod(float secs)
  {
    mPeriod = globalConfig.frameRate * secs;
  }

  void RunningRMS::reset()
  {
    mRMS = 0;
    mCount = 0;
    mSum = 0;
    mSumCount = 0;
  }

  bool RunningRMS::push(float *buffer, int n)
  {
    float *end = buffer + n;

    // calculate sum(x*x)
    float32x4_t sum = vdupq_n_f32(0);
    float32x4_t x;

    while (buffer < end)
    {
      x = vld1q_f32(buffer);
      sum = vmlaq_f32(sum, x, x);
      buffer += 4;
    }

    float tmp[4];
    vst1q_f32(tmp, sum);
    mSum += tmp[0] + tmp[1] + tmp[2] + tmp[3];
    mSumCount += n;
    mCount++;

    if (mCount >= mPeriod)
    {
      // convert to RMS
      mRMS = sqrtf(mSum / mSumCount);
      mCount = 0;
      mSum = 0;
      mSumCount = 0;
      return true;
    }
    else
    {
      return false;
    }
  }

} /* namespace od */
