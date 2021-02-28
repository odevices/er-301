/*
 * Loudness.cpp
 *
 *  Created on: 4 Sep 2016
 *      Author: clarkson
 */

#include <od/extras/Loudness.h>
#include <hal/simd.h>
#include <od/config.h>
#include <hal/ops.h>
#include <math.h>

namespace od
{

  Loudness::Loudness()
  {
    mPeriod = globalConfig.frameRate * GRAPHICS_REFRESH_PERIOD;
  }

  Loudness::~Loudness()
  {
    // TODO Auto-generated destructor stub
  }

  void Loudness::reset()
  {
    mCount = 0;
    mMaximum = 0;
    mDecibels = -160;
  }

  float Loudness::push(float *buffer, int n)
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
      if (mMaximum > 0)
      {
        mDecibels = MAX(-160, 20 * log10f(mMaximum));
      }
      else
      {
        mDecibels = -160;
      }
      mCount = 0;
      // forget
      mNextMax = 0;
    }
    else
    {
      mNextMax = MAX(s3, mNextMax);
    }
    return mDecibels;
  }

} /* namespace od */
