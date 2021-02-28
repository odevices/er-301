/*
 * SkewedSineEnvelope.cpp
 *
 *  Created on: 7 Jun 2017
 *      Author: clarkson
 */

#include <core/objects/env/SkewedSineEnvelope.h>
#include <math.h>
#include <od/config.h>
#include <hal/simd.h>
#include <hal/ops.h>

namespace od
{

  SkewedSineEnvelope::SkewedSineEnvelope()
  {
    addInput(mTrigger);
    addInput(mLevel);
    addParameter(mDuration);
    addParameter(mSkew);
    addOutput(mOutput);
  }

  SkewedSineEnvelope::~SkewedSineEnvelope()
  {
  }

  float SkewedSineEnvelope::startEnvelope()
  {
    float skew = mSkew.value();
    float duration = MAX(0.001f, mDuration.value());
    mCount = duration * globalConfig.sampleRate;
    mCount = 4 * (mCount / 4);

    // [-1,1] => [0 + eps, 0.5 - eps]
#define EPS 0.01f
    if (skew < -1.0f)
    {
      skew = EPS;
    }
    else if (skew > 1.0f)
    {
      skew = 0.5f - EPS;
    }
    else
    {
      skew = 0.5f * (0.5f - 2 * EPS) * (skew + 1.0f) + EPS;
    }
    if (skew < 0.25f)
    {
      mPhaseDelta = 0.5f / mCount;
      mK1 = 0.25f / skew;
      mK2 = 0.25f / (0.5f - skew);
      mC2 = 0.5f * (1.0f - mK2);
      return 0.0f;
    }
    else
    {
      mPhaseDelta = -0.5f / mCount;
      mK1 = 0.25f / (0.5f - skew);
      mK2 = 0.25f / skew;
      mC2 = 0.5f * (1.0f - mK2);
      return 0.5f;
    }
  }

  void SkewedSineEnvelope::process()
  {
    float *trig = mTrigger.buffer();
    float *level = mLevel.buffer();
    float *out = mOutput.buffer();
    float32x4_t twopi = vdupq_n_f32(2.0f * M_PI);
    float32x4_t k1 = vdupq_n_f32(mK1);
    float32x4_t k2 = vdupq_n_f32(mK2);
    float32x4_t c2 = vdupq_n_f32(mC2);
    float32x4_t d = vdupq_n_f32(4 * mPhaseDelta);
    float32x4_t fd = vdupq_n_f32(4 * mFadeDelta);
    float32x4_t zero = vdupq_n_f32(0.0f);

    for (int i = 0; i < FRAMELENGTH; i += 4)
    {
      bool triggerDetected = trig[i] > 0.5f || trig[i + 1] > 0.5f || trig[i + 2] > 0.5f || trig[i + 3] > 0.5f;

      if (triggerDetected)
      {
        mState = triggered;
        mPhases[0] = startEnvelope();
        mPhases[1] = mPhases[0] + mPhaseDelta;
        mPhases[2] = mPhases[1] + mPhaseDelta;
        mPhases[3] = mPhases[2] + mPhaseDelta;
        k1 = vdupq_n_f32(mK1);
        k2 = vdupq_n_f32(mK2);
        c2 = vdupq_n_f32(mC2);
        d = vdupq_n_f32(4 * mPhaseDelta);
        mFades[0] = mLasts[3];
        mFadeDelta = -mLasts[3] / mCount;
        mFades[1] = mFades[0] + mFadeDelta;
        mFades[2] = mFades[1] + mFadeDelta;
        mFades[3] = mFades[2] + mFadeDelta;
        fd = vdupq_n_f32(4 * mFadeDelta);
      }

      if (mCount > 0)
      {

        // calculation for fade ramp
        float32x4_t f = vld1q_f32(mFades) + fd;
        vst1q_f32(mFades, f);

        // calculation for skewed sine envelope
        float32x4_t ep = vld1q_f32(mPhases) + d;
        vst1q_f32(mPhases, ep);
        // skew
        float32x4_t x1 = k1 * ep;
        float32x4_t x2 = k2 * ep + c2;
        ep = vminq_f32(x1, x2);
        // scale to [-pi,pi]
        float32x4_t e = vmaxq_f32(f, simd_sin(twopi * ep));
        vst1q_f32(mLasts, e);
        float32x4_t g = vld1q_f32(level + i);
        vst1q_f32(out + i, e * g);

        mCount -= 4;
        if (mCount == 0)
        {
          mState = waiting;
        }
      }
      else
      {
        vst1q_f32(out + i, zero);
        mLasts[3] = 0.0f;
      }
    }
  }

} /* namespace od */
