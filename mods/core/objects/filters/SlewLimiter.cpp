/*
 * SlewLimiter.cpp
 *
 *  Created on: 17 Dec 2017
 *      Author: clarkson
 */

#include <core/objects/filters/SlewLimiter.h>
#include <od/extras/LookupTables.h>
#include <od/config.h>
#include <hal/simd.h>
#include <hal/ops.h>

#define LOCAL_USE_NEON 1

namespace od
{

  SlewLimiter::SlewLimiter()
  {
    addInput(mInput);
    addOutput(mOutput);
    addParameter(mTime);
    addOption(mDirection);
  }

  SlewLimiter::~SlewLimiter()
  {
  }

  void SlewLimiter::process()
  {
    float *in = mInput.buffer();
    float *out = mOutput.buffer();
    float *ramp = LookupTables::FrameOfLinearRamp.mValues.data();
    float rate = 1.0f / CLAMP(0.003, 1000, mTime.value());
    float maxDiff = rate * globalConfig.framePeriod;

    float finalValue = 0.0f;
#if LOCAL_USE_NEON
    float32x4_t s = vdupq_n_f32(0);
    for (int i = 0; i < FRAMELENGTH; i += 4)
    {
      s += vld1q_f32(in + i);
    }
    finalValue += vgetq_lane_f32(s, 0);
    finalValue += vgetq_lane_f32(s, 1);
    finalValue += vgetq_lane_f32(s, 2);
    finalValue += vgetq_lane_f32(s, 3);
#else
    for (int i = 0; i < FRAMELENGTH; i++)
    {
      finalValue += in[i];
    }
#endif
    finalValue /= FRAMELENGTH;

    float diff;
    switch (mDirection.value())
    {
    case CHOICE_UP:
      if (finalValue > mPreviousValue)
      {
        diff = MIN(maxDiff, finalValue - mPreviousValue);
      }
      else
      {
        diff = finalValue - mPreviousValue;
      }
      break;
    case CHOICE_BOTH:
      diff = CLAMP(-maxDiff, maxDiff, finalValue - mPreviousValue);
      break;
    case CHOICE_DOWN:
      if (finalValue < mPreviousValue)
      {
        diff = MAX(-maxDiff, finalValue - mPreviousValue);
      }
      else
      {
        diff = finalValue - mPreviousValue;
      }
      break;
    }

#if LOCAL_USE_NEON
    float32x4_t p = vdupq_n_f32(mPreviousValue);
    float32x4_t d = vdupq_n_f32(diff);
    for (int i = 0; i < FRAMELENGTH; i += 4)
    {
      vst1q_f32(out + i, p + d * vld1q_f32(ramp + i));
    }
#else
    for (int i = 0; i < FRAMELENGTH; i++)
    {
      out[i] = mPreviousValue + diff * ramp[i];
    }
#endif

    mPreviousValue = out[FRAMELENGTH - 1];
  }

} /* namespace od */
