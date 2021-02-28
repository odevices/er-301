#include <stdlib.h>
#include <core/objects/heads/LoopHead.h>
#include <od/AudioThread.h>
#include <hal/simd.h>
#include <hal/ops.h>
#include <hal/simd.h>
#include <math.h>

namespace od
{

  LoopHead::LoopHead(int channelCount) : mOutputChannelCount(channelCount)
  {
    addInput(mSyncInput);
    addInput(mSpeedInput);
    addOutput(mLeftOutput);
    if (channelCount > 1)
    {
      addOutput(mRightOutput);
    }
    addParameter(mLoopStart);
    addParameter(mLoopLength);
    addParameter(mFadeTime);
    addParameter(mMaxLoopDuration);
    addParameter(mActualLoopDuration);
    addOption(mInterpolation);
    addOption(mRouting);
  }

  LoopHead::~LoopHead()
  {
  }

  void LoopHead::setSample(Sample *sample)
  {
    Base::setSample(sample);

    float *left = mLeftOutput.buffer();
    memset(left, 0, globalConfig.frameLength * sizeof(float));

    if (mOutputChannelCount > 1)
    {
      float *right = mRightOutput.buffer();
      memset(right, 0, globalConfig.frameLength * sizeof(float));
    }

    mPhase = 0.0f;

    if (mpSample)
    {
      if (fabs(mpSample->mSampleRate - globalConfig.sampleRate) < 0.5f)
      {
        mSpeedAdjustment = 1.0f;
      }
      else
      {
        mSpeedAdjustment = sample->mSampleRate * globalConfig.samplePeriod;
      }
    }
    else
    {
      mSpeedAdjustment = 1.0f;
    }

    mFade.reset(0, 1);
  }

  void LoopHead::process()
  {
    if (mpSample == 0)
    {
      return;
    }

    int ti = simd_first_positive_4x(mSyncInput.buffer(), FRAMELENGTH);
    mFade.setLength((int)(globalConfig.sampleRate * mFadeTime.value()));

    if (mResetRequested)
    {
      mResetRequested = false;
      ti = 0;
    }

    int loopStart = (int)(mLoopStart.value() * mpSample->mSampleCount);
    loopStart = CLAMP(0, (int)mpSample->mSampleCount - 1, loopStart);

    int maxLoopLength;
    if (mMaxLoopDuration.value() > 35.9f)
    {
      maxLoopLength = mpSample->mSampleCount;
    }
    else
    {
      maxLoopLength = (int)(mMaxLoopDuration.value() * mpSample->mSampleRate);
    }

    int loopLength = (int)(mLoopLength.value() * maxLoopLength);

    if (loopLength > 0)
    {
      loopLength = MIN(loopLength, maxLoopLength);
    }
    else
    {
      loopLength = MAX(loopLength, -maxLoopLength);
    }

    int loopEnd = loopStart + loopLength;
    loopEnd = CLAMP(0, (int)mpSample->mSampleCount - 1, loopEnd);

    float duration = mpSample->calculateDurationRobustly(abs(loopEnd - loopStart));
    mActualLoopDuration.hardSet(duration);

    float *speed;
    float *tmp = 0;
    if (mPaused)
    {
      speed = ZeroOutput.buffer();
    }
    else
    {
      float speedAdjustment;
      if (loopStart < loopEnd)
      {
        speedAdjustment = mSpeedAdjustment;
      }
      else
      {
        speedAdjustment = -mSpeedAdjustment;
        int swap = loopStart;
        loopStart = loopEnd;
        loopEnd = swap;
      }

      speed = mSpeedInput.buffer();
      tmp = AudioThread::getFrame();
      for (int i = 0; i < FRAMELENGTH; i++)
      {
        tmp[i] = speed[i] * speedAdjustment;
      }
      speed = tmp;
    }

    if (mCurrentIndex < loopStart)
    {
      if (mpSample->mChannelCount == 1)
      {
        jumpToMono(loopEnd);
      }
      else
      {
        jumpToStereo(loopEnd);
      }
    }

    if (mCurrentIndex > loopEnd)
    {
      if (mpSample->mChannelCount == 1)
      {
        jumpToMono(loopStart);
      }
      else
      {
        jumpToStereo(loopStart);
      }
    }

    if (mpSample->mChannelCount == 1)
    {
      if (mOutputChannelCount == 1)
      {
        synthesizeMono(loopStart, loopEnd, ti, speed, mLeftOutput.buffer());
      }
      else
      {
        float *left = mLeftOutput.buffer();
        float *right = mRightOutput.buffer();
        synthesizeMono(loopStart, loopEnd, ti, speed, left);
        memcpy(right, left, FRAMELENGTH * sizeof(float));
      }
    }
    else
    {
      if (mOutputChannelCount == 1)
      {
        float *out = mLeftOutput.buffer();
        float *other = AudioThread::getFrame();
        switch (mRouting.value())
        {
        case CHOICE_LEFT:
          synthesizeMono(loopStart, loopEnd, ti, speed, out);
          break;
        case CHOICE_BOTH:
          synthesizeStereo(loopStart, loopEnd, ti, speed, out, other);
          for (int i = 0; i < FRAMELENGTH; i++)
          {
            out[i] = 0.5f * (out[i] + other[i]);
          }
          break;
        case CHOICE_RIGHT:
          synthesizeStereo(loopStart, loopEnd, ti, speed, other, out);
          break;
        }
        AudioThread::releaseFrame(other);
      }
      else
      {
        float *left = mLeftOutput.buffer();
        float *right = mRightOutput.buffer();
        synthesizeStereo(loopStart, loopEnd, ti, speed, left, right);
      }
    }

    if (tmp)
    {
      AudioThread::releaseFrame(tmp);
    }
  }

  void LoopHead::jumpToMono(int next)
  {
    if (next != mCurrentIndex)
    {
      float w1 = mFade.mValue;
      float w2 = 1.0f - w1;
      mLeftBias = w1 * mpSample->get(mCurrentIndex, 0) + w2 * mLeftBias;
      mFade.reset(0, 1);
      mCurrentIndex = next;
    }
  }

  void LoopHead::jumpToStereo(int next)
  {
    if (next != mCurrentIndex)
    {
      float w1 = mFade.mValue;
      float w2 = 1.0f - w1;
      mLeftBias = w1 * mpSample->get(mCurrentIndex, 0) + w2 * mLeftBias;
      mRightBias = w1 * mpSample->get(mCurrentIndex, 1) + w2 * mRightBias;
      mFade.reset(0, 1);
      mCurrentIndex = next;
    }
  }

  void LoopHead::synthesizeMono(int loopStart, int loopEnd, int ti, float *speed,
                                float *left)
  {
    // neon state for interpolation
    float phase[4] = {0.0f, 0.0f, 0.0f, 0.0f};
    float recentL0[4] = {0.0f, 0.0f, 0.0f, 0.0f};
    float recentL1[4] = {0.0f, 0.0f, 0.0f, 0.0f};
    float recentL2[4] = {0.0f, 0.0f, 0.0f, 0.0f};
    int interpolation = mInterpolation.value();
    int i;

    // Use previous behavior up until ti
    for (i = 0; i < ti; i += 4)
    {
      for (int j = 0; j < 4; j++)
      {
        mPhase += speed[i + j];
        int k = (int)mPhase;
        mPhase -= k;
        phase[j] = mPhase;
        pushMono(k, loopStart, loopEnd);
        recentL0[j] = mLeftFifo[0];
        recentL1[j] = mLeftFifo[1];
        recentL2[j] = mLeftFifo[2];
      }
      switch (interpolation)
      {
      case INTERPOLATION_NONE:
        left[i] = recentL0[0];
        left[i + 1] = recentL0[1];
        left[i + 2] = recentL0[2];
        left[i + 3] = recentL0[3];
        break;
      case INTERPOLATION_LINEAR:
        simd_linear_interpolate(left + i, recentL0, recentL1, phase);
        break;
      case INTERPOLATION_QUADRATIC:
        simd_quadratic_interpolate(left + i, recentL0, recentL1, recentL2,
                                   phase);
        break;
      }
    }

    if (ti < FRAMELENGTH)
    {
      if (speed[ti] < 0.0f)
      {
        jumpToMono(loopEnd);
      }
      else
      {
        jumpToMono(loopStart);
      }
    }

    for (; i < FRAMELENGTH; i += 4)
    {

      for (int j = 0; j < 4; j++)
      {
        mPhase += speed[i + j];
        int k = (int)mPhase;
        mPhase -= k;
        phase[j] = mPhase;
        pushMono(k, loopStart, loopEnd);
        recentL0[j] = mLeftFifo[0];
        recentL1[j] = mLeftFifo[1];
        recentL2[j] = mLeftFifo[2];
      }
      switch (interpolation)
      {
      case INTERPOLATION_NONE:
        left[i] = recentL0[0];
        left[i + 1] = recentL0[1];
        left[i + 2] = recentL0[2];
        left[i + 3] = recentL0[3];
        break;
      case INTERPOLATION_LINEAR:
        simd_linear_interpolate(left + i, recentL0, recentL1, phase);
        break;
      case INTERPOLATION_QUADRATIC:
        simd_quadratic_interpolate(left + i, recentL0, recentL1, recentL2,
                                   phase);
        break;
      }
    }
  }

  void LoopHead::synthesizeStereo(int loopStart, int loopEnd, int ti,
                                  float *speed, float *left, float *right)
  {
    // neon state for interpolation
    float phase[4] = {0.0f, 0.0f, 0.0f, 0.0f};
    float recentL0[4] = {0.0f, 0.0f, 0.0f, 0.0f};
    float recentL1[4] = {0.0f, 0.0f, 0.0f, 0.0f};
    float recentL2[4] = {0.0f, 0.0f, 0.0f, 0.0f};
    float recentR0[4] = {0.0f, 0.0f, 0.0f, 0.0f};
    float recentR1[4] = {0.0f, 0.0f, 0.0f, 0.0f};
    float recentR2[4] = {0.0f, 0.0f, 0.0f, 0.0f};
    int interpolation = mInterpolation.value();
    int i;

    // Use previous behavior up until ti
    for (i = 0; i < ti; i += 4)
    {
      for (int j = 0; j < 4; j++)
      {
        mPhase += speed[i + j];
        int k = (int)mPhase;
        mPhase -= k;
        phase[j] = mPhase;
        pushStereo(k, loopStart, loopEnd);
        recentL0[j] = mLeftFifo[0];
        recentL1[j] = mLeftFifo[1];
        recentL2[j] = mLeftFifo[2];
        recentR0[j] = mRightFifo[0];
        recentR1[j] = mRightFifo[1];
        recentR2[j] = mRightFifo[2];
      }
      switch (interpolation)
      {
      case INTERPOLATION_NONE:
        left[i] = recentL0[0];
        left[i + 1] = recentL0[1];
        left[i + 2] = recentL0[2];
        left[i + 3] = recentL0[3];
        right[i] = recentR0[0];
        right[i + 1] = recentR0[1];
        right[i + 2] = recentR0[2];
        right[i + 3] = recentR0[3];
        break;
      case INTERPOLATION_LINEAR:
        simd_linear_interpolate(left + i, recentL0, recentL1, phase);
        simd_linear_interpolate(right + i, recentR0, recentR1, phase);
        break;
      case INTERPOLATION_QUADRATIC:
        simd_quadratic_interpolate(left + i, recentL0, recentL1, recentL2,
                                   phase);
        simd_quadratic_interpolate(right + i, recentR0, recentR1, recentR2,
                                   phase);
        break;
      }
    }

    if (ti < FRAMELENGTH)
    {
      if (speed[ti] < 0.0f)
      {
        jumpToStereo(loopEnd);
      }
      else
      {
        jumpToStereo(loopStart);
      }
    }

    for (; i < FRAMELENGTH; i += 4)
    {

      for (int j = 0; j < 4; j++)
      {
        mPhase += speed[i + j];
        int k = (int)mPhase;
        mPhase -= k;
        phase[j] = mPhase;
        pushStereo(k, loopStart, loopEnd);
        recentL0[j] = mLeftFifo[0];
        recentL1[j] = mLeftFifo[1];
        recentL2[j] = mLeftFifo[2];
        recentR0[j] = mRightFifo[0];
        recentR1[j] = mRightFifo[1];
        recentR2[j] = mRightFifo[2];
      }
      switch (interpolation)
      {
      case INTERPOLATION_NONE:
        left[i] = recentL0[0];
        left[i + 1] = recentL0[1];
        left[i + 2] = recentL0[2];
        left[i + 3] = recentL0[3];
        right[i] = recentR0[0];
        right[i + 1] = recentR0[1];
        right[i + 2] = recentR0[2];
        right[i + 3] = recentR0[3];
        break;
      case INTERPOLATION_LINEAR:
        simd_linear_interpolate(left + i, recentL0, recentL1, phase);
        simd_linear_interpolate(right + i, recentR0, recentR1, phase);
        break;
      case INTERPOLATION_QUADRATIC:
        simd_quadratic_interpolate(left + i, recentL0, recentL1, recentL2,
                                   phase);
        simd_quadratic_interpolate(right + i, recentR0, recentR1, recentR2,
                                   phase);
        break;
      }
    }
  }

  void LoopHead::pushStereo(int k, int loopStart, int loopEnd)
  {
    while (k > 3)
    {
      int x = k - 3;
      int p = mCurrentIndex + x;
      if (p > loopEnd && mCurrentIndex <= loopEnd)
      {
        // passing through end
        x = loopEnd - mCurrentIndex + 1;
        k -= x;
        jumpToStereo(loopStart);
      }
      else
      {
        k -= x;
        mCurrentIndex = p;
        break;
      }
    }

    while (k < -3)
    {
      int x = k + 3;
      int p = mCurrentIndex + x;
      if (p < loopStart && mCurrentIndex >= loopStart)
      {
        // passing through start
        x = loopStart - mCurrentIndex - 1;
        k -= x;
        jumpToStereo(loopEnd);
      }
      else
      {
        k -= x;
        mCurrentIndex = p;
        break;
      }
    }

    // going backwards
    while (k < 0)
    {
      k++;

      // consume next sample
      mLeftFifo[2] = mLeftFifo[1];
      mLeftFifo[1] = mLeftFifo[0];

      mRightFifo[2] = mRightFifo[1];
      mRightFifo[1] = mRightFifo[0];

      float w1 = mFade.step();
      float w2 = 1.0f - w1;
      mLeftFifo[0] = w1 * mpSample->get(mCurrentIndex, 0) + w2 * mLeftBias;
      mRightFifo[0] = w1 * mpSample->get(mCurrentIndex, 1) + w2 * mRightBias;

      if (mCurrentIndex <= loopStart)
      {
        jumpToStereo(loopEnd);
      }
      else
      {
        mCurrentIndex--;
      }
    }

    // going forwards
    while (k > 0)
    {
      k--;
      // consume next sample
      mLeftFifo[2] = mLeftFifo[1];
      mLeftFifo[1] = mLeftFifo[0];

      mRightFifo[2] = mRightFifo[1];
      mRightFifo[1] = mRightFifo[0];

      float w1 = mFade.step();
      float w2 = 1.0f - w1;
      mLeftFifo[0] = w1 * mpSample->get(mCurrentIndex, 0) + w2 * mLeftBias;
      mRightFifo[0] = w1 * mpSample->get(mCurrentIndex, 1) + w2 * mRightBias;

      if (mCurrentIndex >= loopEnd)
      {
        jumpToStereo(loopStart);
      }
      else
      {
        mCurrentIndex++;
      }
    }
  }

  void LoopHead::pushMono(int k, int loopStart, int loopEnd)
  {
    while (k > 3)
    {
      int x = k - 3;
      int p = mCurrentIndex + x;
      if (p > loopEnd && mCurrentIndex <= loopEnd)
      {
        // passing through end
        x = loopEnd - mCurrentIndex + 1;
        k -= x;
        jumpToMono(loopStart);
      }
      else
      {
        k -= x;
        mCurrentIndex = p;
        break;
      }
    }

    while (k < -3)
    {
      int x = k + 3;
      int p = mCurrentIndex + x;
      if (p < loopStart && mCurrentIndex >= loopStart)
      {
        // passing through start
        x = loopStart - mCurrentIndex - 1;
        k -= x;
        jumpToMono(loopEnd);
      }
      else
      {
        k -= x;
        mCurrentIndex = p;
        break;
      }
    }

    // going backwards
    while (k < 0)
    {
      k++;

      // consume next sample
      mLeftFifo[2] = mLeftFifo[1];
      mLeftFifo[1] = mLeftFifo[0];

      float w1 = mFade.step();
      float w2 = 1.0f - w1;
      mLeftFifo[0] = w1 * mpSample->get(mCurrentIndex, 0) + w2 * mLeftBias;

      if (mCurrentIndex <= loopStart)
      {
        jumpToMono(loopEnd);
      }
      else
      {
        mCurrentIndex--;
      }
    }

    // going forwards
    while (k > 0)
    {
      k--;
      // consume next sample
      mLeftFifo[2] = mLeftFifo[1];
      mLeftFifo[1] = mLeftFifo[0];

      float w1 = mFade.step();
      float w2 = 1.0f - w1;
      mLeftFifo[0] = w1 * mpSample->get(mCurrentIndex, 0) + w2 * mLeftBias;

      if (mCurrentIndex >= loopEnd)
      {
        jumpToMono(loopStart);
      }
      else
      {
        mCurrentIndex++;
      }
    }
  }

} /* namespace od */
