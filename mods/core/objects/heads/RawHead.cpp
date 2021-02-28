#include <core/objects/heads/RawHead.h>
#include <string.h>

namespace od
{

  RawHead::RawHead(int channelCount) : mOutputChannelCount(channelCount)
  {
    addInput(mTriggerInput);
    addOutput(mLeftOutput);
    if (channelCount > 1)
    {
      addOutput(mRightOutput);
    }
    addOption(mRouting);
    addParameter(mSpeed);
  }

  RawHead::~RawHead()
  {
  }

  void RawHead::setSample(Sample *sample, Slices *slices)
  {
    Base::setSample(sample, slices);
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
    if (mSpeed.roundValue() >= 0)
    {
      setPosition(B.forwardGoal);
    }
    else
    {
      setPosition(B.reverseGoal);
    }
  }

  void RawHead::process()
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

    float step;
    if (mPaused)
    {
      step = 0;
    }
    else
    {
      step = mSpeed.roundValue();
    }

    if (mpSample->mChannelCount == 1)
    {
      float *left = mLeftOutput.buffer();
      synthesizeMono(mPreviousBehavior, B, ti, step, left);

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
          synthesizeMono(mPreviousBehavior, B, ti, step, left);
        }
        break;
        case CHOICE_BOTH:
        {
          float *left = mLeftOutput.buffer();
          float *right = mRightOutput.buffer();
          synthesizeStereo(mPreviousBehavior, B, ti, step, left, right);
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
          synthesizeStereo(mPreviousBehavior, B, ti, step, right, left);
        }
        break;
        }
      }
      else
      {
        float *left = mLeftOutput.buffer();
        float *right = mRightOutput.buffer();
        synthesizeStereo(mPreviousBehavior, B, ti, step, left, right);
      }
    }

    mPreviousBehavior = B;
  }

  // mono versions

  void RawHead::jumpToMono(int next)
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

  void RawHead::synthesizeMono(Behavior &B0, Behavior &B1, int ti, int step,
                               float *out)
  {
    int i;
    for (i = 0; i < ti; i++)
    {
      if (mCurrentIndex >= B0.forwardGoal)
      {
        jumpToMono(B0.forwardJump);
      }
      else if (mCurrentIndex <= B0.reverseGoal)
      {
        jumpToMono(B0.reverseJump);
      }

      float w1 = mFade.step();
      float w2 = 1.0f - w1;
      out[i] = w1 * mpSample->get(mCurrentIndex, 0) + w2 * mLeftBias;

      mCurrentIndex += step;
    }

    if (ti < FRAMELENGTH)
    {
      if (step >= 0)
      {
        jumpToMono(B1.forwardReset);
        float duration = mpSample->calculateDurationRobustly(
            B1.forwardDuration);
        mActualPlayDuration.hardSet(duration);
      }
      else
      {
        jumpToMono(B1.reverseReset);
        float duration = mpSample->calculateDurationRobustly(
            B1.reverseDuration);
        mActualPlayDuration.hardSet(duration);
      }
    }

    for (; i < FRAMELENGTH; i++)
    {
      if (mCurrentIndex >= B1.forwardGoal)
      {
        jumpToMono(B1.forwardJump);
      }
      else if (mCurrentIndex <= B1.reverseGoal)
      {
        jumpToMono(B1.reverseJump);
      }

      float w1 = mFade.step();
      float w2 = 1.0f - w1;
      out[i] = w1 * mpSample->get(mCurrentIndex, 0) + w2 * mLeftBias;

      mCurrentIndex += step;
    }
  }

  // stereo versions

  void RawHead::jumpToStereo(int next)
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

  void RawHead::synthesizeStereo(Behavior &B0, Behavior &B1, int ti, int step,
                                 float *left, float *right)
  {
    int i;
    for (i = 0; i < ti; i++)
    {
      if (mCurrentIndex >= B0.forwardGoal)
      {
        jumpToStereo(B0.forwardJump);
      }
      else if (mCurrentIndex <= B0.reverseGoal)
      {
        jumpToStereo(B0.reverseJump);
      }

      float w1 = mFade.step();
      float w2 = 1.0f - w1;
      left[i] = w1 * mpSample->get(mCurrentIndex, 0) + w2 * mLeftBias;
      right[i] = w1 * mpSample->get(mCurrentIndex, 1) + w2 * mRightBias;

      mCurrentIndex += step;
    }

    if (ti < FRAMELENGTH)
    {
      if (step >= 0)
      {
        jumpToStereo(B1.forwardReset);
        float duration = mpSample->calculateDurationRobustly(
            B1.forwardDuration);
        mActualPlayDuration.hardSet(duration);
      }
      else
      {
        jumpToStereo(B1.reverseReset);
        float duration = mpSample->calculateDurationRobustly(
            B1.reverseDuration);
        mActualPlayDuration.hardSet(duration);
      }
    }

    for (; i < FRAMELENGTH; i++)
    {
      if (mCurrentIndex >= B1.forwardGoal)
      {
        jumpToStereo(B1.forwardJump);
      }
      else if (mCurrentIndex <= B1.reverseGoal)
      {
        jumpToStereo(B1.reverseJump);
      }

      float w1 = mFade.step();
      float w2 = 1.0f - w1;
      left[i] = w1 * mpSample->get(mCurrentIndex, 0) + w2 * mLeftBias;
      right[i] = w1 * mpSample->get(mCurrentIndex, 1) + w2 * mRightBias;

      mCurrentIndex += step;
    }
  }

} /* namespace od */
