/*
 * MinMax.cpp
 *
 *  Created on: 29 Sep 2016
 *      Author: clarkson
 */

#include <od/objects/measurement/MinMax.h>
#include <od/config.h>
#include <hal/simd.h>
#include <hal/ops.h>

namespace od
{

  MinMax::MinMax()
  {
    addInput(mInput);
    addParameter(mMinimum);
    addParameter(mMaximum);
    addParameter(mCenter);
    mPeriod = globalConfig.frameRate * GRAPHICS_REFRESH_PERIOD;
  }

  MinMax::~MinMax()
  {
  }

  void MinMax::process()
  {
    float *in = mInput.buffer();

    if (mInput.isConstant())
    {
      mMinimum.hardSet(in[0]);
      mMaximum.hardSet(in[0]);
      mCenter.hardSet(in[0]);
      mCount = 0;
      // forget
      mNextMin = in[0];
      mNextMax = in[0];
    }
    else
    {
      // calculate min/max/avg of the frame
      float32x4_t x;
      x = vld1q_f32(in);
      float32x4_t max = x;
      float32x4_t min = x;

      for (int i = 4; i < FRAMELENGTH; i += 4)
      {
        x = vld1q_f32(in + i);
        max = vmaxq_f32(max, x);
        min = vminq_f32(min, x);
      }

      float tmp[4];

      vst1q_f32(tmp, min);
      float s2 = MIN(MIN(tmp[0], tmp[1]), MIN(tmp[2], tmp[3]));

      vst1q_f32(tmp, max);
      float s3 = MAX(MAX(tmp[0], tmp[1]), MAX(tmp[2], tmp[3]));

      mCount++;

      if (mCount == mPeriod)
      {
        mMinimum.hardSet(mNextMin);
        mMaximum.hardSet(mNextMax);
        mCenter.hardSet(0.5f * (mNextMin + mNextMax));
        mCount = 0;
        // forget
        mNextMin = s2;
        mNextMax = s3;
      }
      else
      {
        mNextMin = MIN(s2, mNextMin);
        mNextMax = MAX(s3, mNextMax);
      }
    }
  }

} /* namespace od */
