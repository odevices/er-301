#include <core/objects/granular/StereoGrain.h>
#include <hal/simd.h>
#include <od/AudioThread.h>
#include <od/config.h>
#include <hal/ops.h>

namespace od
{

  StereoGrain::StereoGrain()
  {
  }

  StereoGrain::~StereoGrain()
  {
    setSample(0);
  }

  inline void StereoGrain::incrementPhase()
  {
    mPhase += mPhaseDelta;
    int k = (int)mPhase;
    mPhase -= k;

    if (k > 0)
    {
      if (k > 3)
      {
        mCurrentIndex += k - 3;
        k = 3;
      }

      // going forwards
      while (k > 0)
      {
        k--;
        // consume next sample
        mFifoL[2] = mFifoL[1];
        mFifoL[1] = mFifoL[0];
        mFifoR[2] = mFifoR[1];
        mFifoR[1] = mFifoR[0];
        if (mCurrentIndex >= (int)mpSample->mSampleCount)
        {
          mCurrentIndex -= mpSample->mSampleCount;
        }
        mFifoL[0] = mpSample->get(mCurrentIndex, 0);
        mFifoR[0] = mpSample->get(mCurrentIndex, 1);
        mCurrentIndex++;
      }
    }
    else
    {
      k = -k;
      if (k > 3)
      {
        mCurrentIndex -= k - 3;
        k = 3;
      }

      // going forwards
      while (k > 0)
      {
        k--;
        // consume next sample
        mFifoL[2] = mFifoL[1];
        mFifoL[1] = mFifoL[0];
        mFifoR[2] = mFifoR[1];
        mFifoR[1] = mFifoR[0];
        if (mCurrentIndex < 0)
        {
          mCurrentIndex += mpSample->mSampleCount;
        }
        mFifoL[0] = mpSample->get(mCurrentIndex, 0);
        mFifoR[0] = mpSample->get(mCurrentIndex, 1);
        mCurrentIndex--;
      }
    }
  }

  void StereoGrain::synthesize(float *left, float *right)
  {
    if (mRemaining == 0)
    {
      return;
    }

    if (mDelayInSamples >= FRAMELENGTH)
    {
      mDelayInSamples -= FRAMELENGTH;
      return;
    }

    float phase[4] = {0.0f, 0.0f, 0.0f, 0.0f};
    float recentL0[4] = {0.0f, 0.0f, 0.0f, 0.0f};
    float recentL1[4] = {0.0f, 0.0f, 0.0f, 0.0f};
    float recentL2[4] = {0.0f, 0.0f, 0.0f, 0.0f};
    float recentR0[4] = {0.0f, 0.0f, 0.0f, 0.0f};
    float recentR1[4] = {0.0f, 0.0f, 0.0f, 0.0f};
    float recentR2[4] = {0.0f, 0.0f, 0.0f, 0.0f};

    float32x4_t w1 = vdupq_n_f32(mLeftBalance);
    float32x4_t w2 = vdupq_n_f32(mRightBalance);
    int n = MIN(mRemaining, FRAMELENGTH - mDelayInSamples);
    float *env = AudioThread::getFrame();
    generateEnvelope(env + mDelayInSamples, n);
    int N = n + mDelayInSamples;

    for (int i = mDelayInSamples; i < N; i += 4)
    {
      incrementPhase();
      phase[0] = mPhase;
      recentL0[0] = mFifoL[0];
      recentL1[0] = mFifoL[1];
      recentL2[0] = mFifoL[2];
      recentR0[0] = mFifoR[0];
      recentR1[0] = mFifoR[1];
      recentR2[0] = mFifoR[2];

      incrementPhase();
      phase[1] = mPhase;
      recentL0[1] = mFifoL[0];
      recentL1[1] = mFifoL[1];
      recentL2[1] = mFifoL[2];
      recentR0[1] = mFifoR[0];
      recentR1[1] = mFifoR[1];
      recentR2[1] = mFifoR[2];

      incrementPhase();
      phase[2] = mPhase;
      recentL0[2] = mFifoL[0];
      recentL1[2] = mFifoL[1];
      recentL2[2] = mFifoL[2];
      recentR0[2] = mFifoR[0];
      recentR1[2] = mFifoR[1];
      recentR2[2] = mFifoR[2];

      incrementPhase();
      phase[3] = mPhase;
      recentL0[3] = mFifoL[0];
      recentL1[3] = mFifoL[1];
      recentL2[3] = mFifoL[2];
      recentR0[3] = mFifoR[0];
      recentR1[3] = mFifoR[1];
      recentR2[3] = mFifoR[2];

      float32x4_t x;
      float32x4_t e = vld1q_f32(env + i);

      x = simd_quadratic_interpolate_with_return(recentL0, recentL1, recentL2,
                                                 phase);
      vst1q_f32(left + i, vmlaq_f32(vld1q_f32(left + i), w1, x * e));

      x = simd_quadratic_interpolate_with_return(recentR0, recentR1, recentR2,
                                                 phase);
      vst1q_f32(right + i, vmlaq_f32(vld1q_f32(right + i), w2, x * e));
    }

    mDelayInSamples = 0;
    mRemaining -= n;
    mActive = mRemaining > 0;
    mLastEnvelopeValue = env[FRAMELENGTH - 1];
    AudioThread::releaseFrame(env);
  }

} /* namespace od */
