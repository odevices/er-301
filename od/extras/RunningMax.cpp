/*
 * RunningMax.cpp
 *
 *  Created on: 7 Oct 2016
 *      Author: clarkson
 */

#include <od/extras/RunningMax.h>
#include <od/config.h>
#include <hal/simd.h>
#include <hal/ops.h>

namespace od
{

  RunningMax::RunningMax()
  {
    mPeriod = globalConfig.frameRate * 0.01f;
  }

  RunningMax::~RunningMax()
  {
  }

  void RunningMax::setPeriod(float secs)
  {
    mPeriod = globalConfig.frameRate * secs;
  }

  void RunningMax::reset()
  {
    mMaximum = 0;
    mCount = 0;
    mNextMax = 0;
  }

  bool RunningMax::push(float *buffer, int n)
  {
    float *end = buffer + n;

    // calculate min/max/avg of the frame
    float32x4_t x;
    x = vabsq_f32(vld1q_f32(buffer));

    float32x4_t max = x;
    buffer += 4;

    while (buffer < end)
    {
      x = vabsq_f32(vld1q_f32(buffer));
      max = vmaxq_f32(max, x);
      buffer += 4;
    }

    float tmp[4];
    vst1q_f32(tmp, max);
    float s3 = MAX(MAX(tmp[0], tmp[1]), MAX(tmp[2], tmp[3]));

    mCount++;

    if (mCount >= mPeriod)
    {
      mMaximum = mNextMax;
      mCount = 0;
      // forget
      mNextMax = 0;
      return true;
    }
    else
    {
      mNextMax = MAX(s3, mNextMax);
      return false;
    }
  }

} /* namespace od */
