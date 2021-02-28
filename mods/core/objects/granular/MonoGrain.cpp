#include <core/objects/granular/MonoGrain.h>
#include <hal/simd.h>
#include <od/AudioThread.h>
#include <od/config.h>
#include <hal/ops.h>

namespace od
{

  MonoGrain::MonoGrain()
  {
  }

  MonoGrain::~MonoGrain()
  {
  }

  inline void MonoGrain::incrementPhaseOnMono()
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
        mFifo[2] = mFifo[1];
        mFifo[1] = mFifo[0];
        if (mCurrentIndex >= (int)mpSample->mSampleCount)
        {
          mCurrentIndex -= mpSample->mSampleCount;
        }
        mFifo[0] = mpSample->getMonoFromMono(mCurrentIndex);
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
        mFifo[2] = mFifo[1];
        mFifo[1] = mFifo[0];
        if (mCurrentIndex < 0)
        {
          mCurrentIndex += mpSample->mSampleCount;
        }
        mFifo[0] = mpSample->getMonoFromMono(mCurrentIndex);
        mCurrentIndex--;
      }
    }
  }

  void MonoGrain::synthesizeFromMonoToMono(float *out)
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
    float recent0[4] = {0.0f, 0.0f, 0.0f, 0.0f};
    float recent1[4] = {0.0f, 0.0f, 0.0f, 0.0f};
    float recent2[4] = {0.0f, 0.0f, 0.0f, 0.0f};

    float32x4_t g = vdupq_n_f32(mLeftBalance);
    int n = MIN(mRemaining, FRAMELENGTH - mDelayInSamples);
    float *env = AudioThread::getFrame();
    generateEnvelope(env + mDelayInSamples, n);
    int N = n + mDelayInSamples;

    for (int i = mDelayInSamples; i < N; i += 4)
    {
      incrementPhaseOnMono();
      phase[0] = mPhase;
      recent0[0] = mFifo[0];
      recent1[0] = mFifo[1];
      recent2[0] = mFifo[2];

      incrementPhaseOnMono();
      phase[1] = mPhase;
      recent0[1] = mFifo[0];
      recent1[1] = mFifo[1];
      recent2[1] = mFifo[2];

      incrementPhaseOnMono();
      phase[2] = mPhase;
      recent0[2] = mFifo[0];
      recent1[2] = mFifo[1];
      recent2[2] = mFifo[2];

      incrementPhaseOnMono();
      phase[3] = mPhase;
      recent0[3] = mFifo[0];
      recent1[3] = mFifo[1];
      recent2[3] = mFifo[2];

      float32x4_t x = simd_quadratic_interpolate_with_return(recent0, recent1,
                                                             recent2, phase);
      x *= vld1q_f32(env + i);
      float32x4_t o = vld1q_f32(out + i);
      vst1q_f32(out + i, vmlaq_f32(o, g, x));
    }

    mDelayInSamples = 0;
    mRemaining -= n;
    mActive = mRemaining > 0;
    AudioThread::releaseFrame(env);
  }

  void MonoGrain::synthesizeFromMonoToStereo(float *left, float *right)
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
    float recent0[4] = {0.0f, 0.0f, 0.0f, 0.0f};
    float recent1[4] = {0.0f, 0.0f, 0.0f, 0.0f};
    float recent2[4] = {0.0f, 0.0f, 0.0f, 0.0f};

    float32x4_t w1 = vdupq_n_f32(mLeftBalance);
    float32x4_t w2 = vdupq_n_f32(mRightBalance);
    int n = MIN(mRemaining, FRAMELENGTH - mDelayInSamples);
    float *env = AudioThread::getFrame();
    generateEnvelope(env + mDelayInSamples, n);
    int N = n + mDelayInSamples;

    for (int i = mDelayInSamples; i < N; i += 4)
    {
      incrementPhaseOnMono();
      phase[0] = mPhase;
      recent0[0] = mFifo[0];
      recent1[0] = mFifo[1];
      recent2[0] = mFifo[2];

      incrementPhaseOnMono();
      phase[1] = mPhase;
      recent0[1] = mFifo[0];
      recent1[1] = mFifo[1];
      recent2[1] = mFifo[2];

      incrementPhaseOnMono();
      phase[2] = mPhase;
      recent0[2] = mFifo[0];
      recent1[2] = mFifo[1];
      recent2[2] = mFifo[2];

      incrementPhaseOnMono();
      phase[3] = mPhase;
      recent0[3] = mFifo[0];
      recent1[3] = mFifo[1];
      recent2[3] = mFifo[2];

      float32x4_t x = simd_quadratic_interpolate_with_return(recent0, recent1,
                                                             recent2, phase);
      x *= vld1q_f32(env + i);
      float32x4_t L = vld1q_f32(left + i);
      vst1q_f32(left + i, vmlaq_f32(L, w1, x));
      float32x4_t R = vld1q_f32(right + i);
      vst1q_f32(right + i, vmlaq_f32(R, w2, x));
    }

    mDelayInSamples = 0;
    mRemaining -= n;
    mActive = mRemaining > 0;
    AudioThread::releaseFrame(env);
  }

  inline void MonoGrain::incrementPhaseOnStereo()
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
        mFifo[2] = mFifo[1];
        mFifo[1] = mFifo[0];
        if (mCurrentIndex >= (int)mpSample->mSampleCount)
        {
          mCurrentIndex -= mpSample->mSampleCount;
        }
        mFifo[0] = mpSample->getMonoFromStereo(mCurrentIndex);
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
        mFifo[2] = mFifo[1];
        mFifo[1] = mFifo[0];
        if (mCurrentIndex < 0)
        {
          mCurrentIndex += mpSample->mSampleCount;
        }
        mFifo[0] = mpSample->getMonoFromStereo(mCurrentIndex);
        mCurrentIndex--;
      }
    }
  }

  void MonoGrain::synthesizeFromStereo(float *out)
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

    float32x4_t g = vdupq_n_f32(mLeftBalance);
    float phase[4] = {0.0f, 0.0f, 0.0f, 0.0f};
    float recent0[4] = {0.0f, 0.0f, 0.0f, 0.0f};
    float recent1[4] = {0.0f, 0.0f, 0.0f, 0.0f};
    float recent2[4] = {0.0f, 0.0f, 0.0f, 0.0f};
    int n = MIN(mRemaining, FRAMELENGTH - mDelayInSamples);
    float *env = AudioThread::getFrame();
    generateEnvelope(env + mDelayInSamples, n);
    int N = n + mDelayInSamples;

    for (int i = mDelayInSamples; i < N; i += 4)
    {
      incrementPhaseOnStereo();
      phase[0] = mPhase;
      recent0[0] = mFifo[0];
      recent1[0] = mFifo[1];
      recent2[0] = mFifo[2];

      incrementPhaseOnStereo();
      phase[1] = mPhase;
      recent0[1] = mFifo[0];
      recent1[1] = mFifo[1];
      recent2[1] = mFifo[2];

      incrementPhaseOnStereo();
      phase[2] = mPhase;
      recent0[2] = mFifo[0];
      recent1[2] = mFifo[1];
      recent2[2] = mFifo[2];

      incrementPhaseOnStereo();
      phase[3] = mPhase;
      recent0[3] = mFifo[0];
      recent1[3] = mFifo[1];
      recent2[3] = mFifo[2];

      float32x4_t x = simd_quadratic_interpolate_with_return(recent0, recent1,
                                                             recent2, phase);
      x *= vld1q_f32(env + i);
      float32x4_t o = vld1q_f32(out + i);
      vst1q_f32(out + i, vmlaq_f32(o, g, x));
    }

    mDelayInSamples = 0;
    mRemaining -= n;
    mActive = mRemaining > 0;
    mLastEnvelopeValue = env[FRAMELENGTH - 1];
    AudioThread::releaseFrame(env);
  }

} /* namespace od */
