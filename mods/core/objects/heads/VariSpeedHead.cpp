#include <core/objects/heads/VariSpeedHead.h>
#include <hal/simd.h>
#include <od/AudioThread.h>

namespace od
{

  VariSpeedHead::VariSpeedHead(int channelCount) : mOutputChannelCount(channelCount)
  {
    addInput(mTriggerInput);
    addInput(mSpeedInput);
    addOutput(mLeftOutput);
    addOutput(mRightOutput);
    addOption(mRouting);
    addOption(mInterpolation);
  }

  VariSpeedHead::~VariSpeedHead()
  {
  }

  void VariSpeedHead::setSample(Sample *sample, Slices *slices)
  {
    Base::setSample(sample, slices);
    mPhase = 0.0f;
    float *left = mLeftOutput.buffer();
    memset(left, 0, globalConfig.frameLength * sizeof(float));

    if (mOutputChannelCount > 1)
    {
      float *right = mRightOutput.buffer();
      memset(right, 0, globalConfig.frameLength * sizeof(float));
    }

    mLeftBias = 0.0f;
    mRightBias = 0.0f;
    mFade.set(1.0f);

    // This sets the play position at the goal of the current behavior.
    // The result is that one-shot behaviors are stopped until a trigger is received.
    Behavior B = determineBehavior(mSustain.value());
    float *speed = mSpeedInput.buffer();
    if (speed[0] >= 0.0f)
    {
      setPosition(B.forwardGoal);
    }
    else
    {
      setPosition(B.reverseGoal);
    }
  }

  void VariSpeedHead::process()
  {
    if (mpSample == 0)
    {
      return;
    }

    int ti = getFirstTrigger(mTriggerInput.buffer());
    if (ti < FRAMELENGTH && mpSlices)
    {
      mpSlices->lock();
      refreshSliceSelection();
      mpSlices->unlock();
    }

    int fadeTime = (int)(globalConfig.sampleRate * mFadeTime.value());
    mFade.setLength(fadeTime);

    if (mResetRequested)
    {
      mResetRequested = false;
      ti = 0;
    }

    Behavior B = determineBehavior(mSustain.value());

    float *speed;
    float *tmp = 0;

    if (mPaused)
    {
      speed = ZeroOutput.buffer();
    }
    else
    {
      speed = mSpeedInput.buffer();
      if (mSpeedAdjustmentEnabled)
      {
        tmp = AudioThread::getFrame();
        for (int i = 0; i < FRAMELENGTH; i++)
        {
          tmp[i] = speed[i] * mSpeedAdjustment;
        }
        speed = tmp;
      }
    }

    if (mpSample->mChannelCount == 1)
    {
      float *left = mLeftOutput.buffer();
      synthesizeMono(mPreviousBehavior, B, ti, speed, left);

      if (mOutputChannelCount > 1)
      {
        float *right = mRightOutput.buffer();
        memcpy(right, left, sizeof(float) * FRAMELENGTH);
      }
    }
    else
    {
      if (mOutputChannelCount == 1)
      {
        switch (mRouting.value())
        {
        case CHOICE_LEFT:
        {
          float *left = mLeftOutput.buffer();
          synthesizeMono(mPreviousBehavior, B, ti, speed, left);
        }
        break;
        case CHOICE_BOTH:
        {
          float *left = mLeftOutput.buffer();
          float *right = mRightOutput.buffer();
          synthesizeStereo(mPreviousBehavior, B, ti, speed, left, right);
          for (int i = 0; i < FRAMELENGTH; i++)
          {
            left[i] = 0.5f * (left[i] + right[i]);
          }
        }
        break;
        case CHOICE_RIGHT:
        {
          float *left = mLeftOutput.buffer();
          float *right = mRightOutput.buffer();
          // throwing away the right buffer
          synthesizeStereo(mPreviousBehavior, B, ti, speed, right, left);
        }
        break;
        }
      }
      else
      {
        float *left = mLeftOutput.buffer();
        float *right = mRightOutput.buffer();
        synthesizeStereo(mPreviousBehavior, B, ti, speed, left, right);
      }
    }

    mPreviousBehavior = B;

    if (tmp)
    {
      AudioThread::releaseFrame(tmp);
    }
  }

  void VariSpeedHead::interpolate(int quality, float *out, float *recent0,
                                  float *recent1, float *recent2, float *phase)
  {
    switch (quality)
    {
    case INTERPOLATION_NONE:
      out[0] = recent0[0];
      out[1] = recent0[1];
      out[2] = recent0[2];
      out[3] = recent0[3];
      break;
    case INTERPOLATION_LINEAR:
      simd_linear_interpolate(out, recent0, recent1, phase);
      break;
    case INTERPOLATION_QUADRATIC:
      simd_quadratic_interpolate(out, recent0, recent1, recent2, phase);
      break;
    }
  }

  // mono versions

  void VariSpeedHead::jumpToMono(int next)
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

  void VariSpeedHead::pushMono(int k, int forwardGoal, int forwardJump,
                               int reverseGoal, int reverseJump)
  {
    while (k > 3)
    {
      int x = k - 3;
      int p = mCurrentIndex + x;
      if (p > forwardGoal && mCurrentIndex <= forwardGoal)
      {
        // passing through end
        x = forwardGoal - mCurrentIndex + 1;
        k -= x;
        jumpToMono(forwardJump);
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
      if (p < reverseGoal && mCurrentIndex >= reverseGoal)
      {
        // passing through start
        x = reverseGoal - mCurrentIndex - 1;
        k -= x;
        jumpToMono(reverseJump);
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
      float w1 = mFade.step();
      float w2 = 1.0f - w1;
      mLeftFifo[2] = mLeftFifo[1];
      mLeftFifo[1] = mLeftFifo[0];
      mLeftFifo[0] = w1 * mpSample->get(mCurrentIndex, 0) + w2 * mLeftBias;

      if (mCurrentIndex <= reverseGoal)
      {
        jumpToMono(reverseJump);
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
      float w1 = mFade.step();
      float w2 = 1.0f - w1;

      mLeftFifo[2] = mLeftFifo[1];
      mLeftFifo[1] = mLeftFifo[0];
      mLeftFifo[0] = w1 * mpSample->get(mCurrentIndex, 0) + w2 * mLeftBias;

      if (mCurrentIndex >= forwardGoal)
      {
        jumpToMono(forwardJump);
      }
      else
      {
        mCurrentIndex++;
      }
    }
  }

  void VariSpeedHead::synthesizeMono(Behavior &B0, Behavior &B1, int ti,
                                     float *speed, float *out)
  {
    // neon state for interpolation
    float phase[4] = {0.0f, 0.0f, 0.0f, 0.0f};
    float recentL0[4] = {0.0f, 0.0f, 0.0f, 0.0f};
    float recentL1[4] = {0.0f, 0.0f, 0.0f, 0.0f};
    float recentL2[4] = {0.0f, 0.0f, 0.0f, 0.0f};
    int quality = mInterpolation.value();
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
        pushMono(k, B0.forwardGoal, B0.forwardJump, B0.reverseGoal,
                 B0.reverseJump);
        recentL0[j] = mLeftFifo[0];
        recentL1[j] = mLeftFifo[1];
        recentL2[j] = mLeftFifo[2];
      }
      interpolate(quality, out + i, recentL0, recentL1, recentL2, phase);
    }

    if (ti < FRAMELENGTH)
    {
      if (speed[ti] < 0.0f)
      {
        jumpToMono(B1.reverseReset);
        float duration = mpSample->calculateDurationRobustly(
            B1.reverseDuration);
        mActualPlayDuration.hardSet(duration);
      }
      else
      {
        jumpToMono(B1.forwardReset);
        float duration = mpSample->calculateDurationRobustly(
            B1.forwardDuration);
        mActualPlayDuration.hardSet(duration);
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
        pushMono(k, B1.forwardGoal, B1.forwardJump, B1.reverseGoal,
                 B1.reverseJump);
        recentL0[j] = mLeftFifo[0];
        recentL1[j] = mLeftFifo[1];
        recentL2[j] = mLeftFifo[2];
      }
      interpolate(quality, out + i, recentL0, recentL1, recentL2, phase);
    }
  }

  // stereo versions

  void VariSpeedHead::jumpToStereo(int next)
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

  void VariSpeedHead::pushStereo(int k, int forwardGoal, int forwardJump,
                                 int reverseGoal, int reverseJump)
  {
    while (k > 3)
    {
      int x = k - 3;
      int p = mCurrentIndex + x;
      if (p > forwardGoal && mCurrentIndex <= forwardGoal)
      {
        // passing through end
        x = forwardGoal - mCurrentIndex + 1;
        k -= x;
        jumpToStereo(forwardJump);
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
      if (p < reverseGoal && mCurrentIndex >= reverseGoal)
      {
        // passing through start
        x = reverseGoal - mCurrentIndex - 1;
        k -= x;
        jumpToStereo(reverseJump);
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
      float w1 = mFade.step();
      float w2 = 1.0f - w1;

      mLeftFifo[2] = mLeftFifo[1];
      mLeftFifo[1] = mLeftFifo[0];
      mLeftFifo[0] = w1 * mpSample->get(mCurrentIndex, 0) + w2 * mLeftBias;

      mRightFifo[2] = mRightFifo[1];
      mRightFifo[1] = mRightFifo[0];
      mRightFifo[0] = w1 * mpSample->get(mCurrentIndex, 1) + w2 * mRightBias;

      if (mCurrentIndex <= reverseGoal)
      {
        jumpToStereo(reverseJump);
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
      float w1 = mFade.step();
      float w2 = 1.0f - w1;

      mLeftFifo[2] = mLeftFifo[1];
      mLeftFifo[1] = mLeftFifo[0];
      mLeftFifo[0] = w1 * mpSample->get(mCurrentIndex, 0) + w2 * mLeftBias;

      mRightFifo[2] = mRightFifo[1];
      mRightFifo[1] = mRightFifo[0];
      mRightFifo[0] = w1 * mpSample->get(mCurrentIndex, 1) + w2 * mRightBias;

      if (mCurrentIndex >= forwardGoal)
      {
        jumpToStereo(forwardJump);
      }
      else
      {
        mCurrentIndex++;
      }
    }
  }

  void VariSpeedHead::synthesizeStereo(Behavior &B0, Behavior &B1, int ti,
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
    int quality = mInterpolation.value();
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
        pushStereo(k, B0.forwardGoal, B0.forwardJump, B0.reverseGoal,
                   B0.reverseJump);
        recentL0[j] = mLeftFifo[0];
        recentL1[j] = mLeftFifo[1];
        recentL2[j] = mLeftFifo[2];
        recentR0[j] = mRightFifo[0];
        recentR1[j] = mRightFifo[1];
        recentR2[j] = mRightFifo[2];
      }
      interpolate(quality, left + i, recentL0, recentL1, recentL2, phase);
      interpolate(quality, right + i, recentR0, recentR1, recentR2, phase);
    }

    if (ti < FRAMELENGTH)
    {
      if (speed[ti] < 0.0f)
      {
        jumpToStereo(B1.reverseReset);
        float duration = mpSample->calculateDurationRobustly(
            B1.reverseDuration);
        mActualPlayDuration.hardSet(duration);
      }
      else
      {
        jumpToStereo(B1.forwardReset);
        float duration = mpSample->calculateDurationRobustly(
            B1.forwardDuration);
        mActualPlayDuration.hardSet(duration);
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
        pushStereo(k, B1.forwardGoal, B1.forwardJump, B1.reverseGoal,
                   B1.reverseJump);
        recentL0[j] = mLeftFifo[0];
        recentL1[j] = mLeftFifo[1];
        recentL2[j] = mLeftFifo[2];
        recentR0[j] = mRightFifo[0];
        recentR1[j] = mRightFifo[1];
        recentR2[j] = mRightFifo[2];
      }
      interpolate(quality, left + i, recentL0, recentL1, recentL2, phase);
      interpolate(quality, right + i, recentR0, recentR1, recentR2, phase);
    }
  }

} /* namespace od */
