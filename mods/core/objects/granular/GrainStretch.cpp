#include <core/objects/granular/GrainStretch.h>
#include <od/extras/Random.h>
#include <hal/simd.h>
#include <hal/ops.h>
#include <algorithm>
#include <string.h>


namespace od
{

  GrainStretch::GrainStretch(int channelCount, int grainCount)
      : mOutputChannelCount(channelCount), mGrainCount(MAX(grainCount, 2))
  {
    addInput(mTriggerInput);
    addInput(mSpeedInput);
    addOutput(mLeftOutput);
    addOutput(mRightOutput);
    addParameter(mGrainDuration);
    addParameter(mGrainPitch);
    addParameter(mGrainJitter);
    setMaximumGrainCount(mGrainCount + 1);
  }

  GrainStretch::~GrainStretch()
  {
  }

  void GrainStretch::setMaximumGrainCount(int n)
  {
    mFreeStereoGrains.clear();
    mFreeStereoGrains.reserve(n);
    mStereoGrains.resize(n);
    // push grains in a reverse memory order for better cache perf
    for (auto i = mStereoGrains.rbegin(); i != mStereoGrains.rend(); i++)
    {
      StereoGrain &grain = *i;
      mFreeStereoGrains.push_back(&grain);
      grain.setEnvelope(Grain::mSineWindow);
      grain.setSquash(2);
    }

    mFreeMonoGrains.clear();
    mFreeMonoGrains.reserve(n);
    mMonoGrains.resize(n);
    // push grains in a reverse memory order for better cache perf
    for (auto i = mMonoGrains.rbegin(); i != mMonoGrains.rend(); i++)
    {
      MonoGrain &grain = *i;
      mFreeMonoGrains.push_back(&grain);
      grain.setEnvelope(Grain::mSineWindow);
      grain.setSquash(2);
    }
  }

  StereoGrain *GrainStretch::getNextFreeStereoGrain()
  {
    if (mEnabled && mFreeStereoGrains.size() > 0)
    {
      StereoGrain *grain = mFreeStereoGrains.back();
      mFreeStereoGrains.pop_back();
      return grain;
    }
    else
    {
      return 0;
    }
  }

  MonoGrain *GrainStretch::getNextFreeMonoGrain()
  {
    if (mEnabled && mFreeMonoGrains.size() > 0)
    {
      MonoGrain *grain = mFreeMonoGrains.back();
      mFreeMonoGrains.pop_back();
      return grain;
    }
    else
    {
      return 0;
    }
  }

  void GrainStretch::stopAllGrains()
  {
    mFreeMonoGrains.clear();
    for (MonoGrain &grain : mMonoGrains)
    {
      if (grain.mActive)
      {
        grain.stop();
      }
      mFreeMonoGrains.push_back(&grain);
    }

    mFreeStereoGrains.clear();
    for (StereoGrain &grain : mStereoGrains)
    {
      if (grain.mActive)
      {
        grain.stop();
      }
      mFreeStereoGrains.push_back(&grain);
    }
  }

  void GrainStretch::setSample(Sample *sample, Slices *slices)
  {
    mEnabled = false;
    stopAllGrains();

    Base::setSample(sample, slices);
    mPhase = 0.0f;

    if (mpSample)
    {
      if (mOutputChannelCount == 2 && mpSample->mChannelCount == 2)
      {
        for (MonoGrain &grain : mMonoGrains)
        {
          grain.setSample(0);
        }
        for (StereoGrain &grain : mStereoGrains)
        {
          grain.setSample(mpSample);
        }
      }
      else
      {
        for (MonoGrain &grain : mMonoGrains)
        {
          grain.setSample(mpSample);
        }
        for (StereoGrain &grain : mStereoGrains)
        {
          grain.setSample(0);
        }
      }
    }
    else
    {
      for (StereoGrain &grain : mStereoGrains)
      {
        grain.setSample(0);
      }

      for (MonoGrain &grain : mMonoGrains)
      {
        grain.setSample(0);
      }
    }

    mEnabled = true;
  }

  void GrainStretch::incrementPhase(int k, int forwardGoal, int forwardJump,
                                    int reverseGoal, int reverseJump)
  {
    int newIndex = mCurrentIndex + k;

    if (k > 0)
    {
      if (newIndex > forwardGoal)
      {
        if (forwardJump < forwardGoal)
        {
          int x = (newIndex - forwardJump) % (forwardGoal - forwardJump);
          newIndex = forwardJump + x;
        }
        else if (forwardJump >= forwardGoal)
        {
          newIndex = forwardJump;
        }
      }
    }
    else if (k < 0)
    {
      if (newIndex < reverseGoal)
      {
        if (reverseJump > reverseGoal)
        {
          int x = (reverseJump - newIndex) % (reverseJump - reverseGoal);
          newIndex = reverseJump - x;
        }
        else if (reverseJump <= reverseGoal)
        {
          newIndex = reverseJump;
        }
      }
    }

    mCurrentIndex = newIndex;
  }

  void GrainStretch::increment(Behavior &B, float delta)
  {
    mPhase += delta * mSpeedAdjustment;
    int k = (int)mPhase;
    mPhase -= k;
    incrementPhase(k, B.forwardGoal, B.forwardJump, B.reverseGoal,
                   B.reverseJump);
  }

  static inline float computeSum(float *frame)
  {
    float sum = 0;
    for (int i = 0; i < FRAMELENGTH; i++)
    {
      sum += frame[i];
    }
    return sum;
  }

  void GrainStretch::process()
  {
    switch (mOutputChannelCount)
    {
    case 1:
      memset(mLeftOutput.buffer(), 0, globalConfig.frameLength * sizeof(float));
      break;
    case 2:
      memset(mLeftOutput.buffer(), 0, globalConfig.frameLength * sizeof(float));
      memset(mRightOutput.buffer(), 0, globalConfig.frameLength * sizeof(float));
      break;
    }

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

    Behavior B = determineBehavior(mSustain.value());

    if (mResetRequested || ti < FRAMELENGTH)
    {
      mResetRequested = false;
      float speed = mSpeedInput.buffer()[0];
      if (speed < 0.0f)
      {
        mCurrentIndex = B.reverseReset;
        mActualPlayDuration.hardSet(
            mpSample->calculateDurationRobustly(B.reverseDuration));
      }
      else
      {
        mCurrentIndex = B.forwardReset;
        mActualPlayDuration.hardSet(
            mpSample->calculateDurationRobustly(B.forwardDuration));
      }
      mFramesUntilNextGrain = 0;
    }

    if (mOutputChannelCount == 2 && mpSample->mChannelCount == 2)
    {
      processStereoToStereo(B);
    }
    else
    {
      processAnyMono(B);
    }
  }

  void GrainStretch::produceGrain(Behavior &B, bool mono)
  {
    if (!mPaused)
    {
      float speed = mSpeedInput.buffer()[0];

      if (mFramesUntilNextGrain == 0)
      {
        bool reverse = speed < 0.0f;
        // Grain period must be a multiple of frames and at least one frame.
        int grainPeriod = MAX(
            1, 0.5f * mGrainDuration.value() * globalConfig.frameRate);
        mFramesUntilNextGrain = grainPeriod;

        if (mLastGrainIndex != mCurrentIndex && speed != 0.0f)
        {
          // create a grain
          Grain *grain = 0;
          if (mono)
          {
            grain = getNextFreeMonoGrain();
          }
          else
          {
            grain = getNextFreeStereoGrain();
          }

          if (grain)
          {
            float octave = MAX(-1, MIN(1, mGrainPitch.value()));
            float speedG = powf(2.0f, octave * FULLSCALE_IN_VOLTS);
            // Requirement for 50% overlap:
            // 2 * speedG * durG = avgSpeed * grainPeriod
            float durAdj = MAX(1.0f, fabs(speed / speedG));
            int duration = MIN(
                globalConfig.sampleRate,
                2.0f * durAdj * grainPeriod * FRAMELENGTH);
            //float gain = 1.0f / sqrtf(durAdj);
            float gain = 0.77f;

            // Prevent the grain from exceeding the goal.
            int samplesReq = duration * speedG * grain->mSpeedAdjustment;
            if (reverse)
            {
              int samplesAvail = abs(B.reverseGoal - mCurrentIndex);
              if (samplesAvail < samplesReq && samplesReq > 0)
              {
                duration *= (float)samplesAvail / (float)samplesReq;
              }
              grain->init(mCurrentIndex, duration, -speedG, gain,
                          0.0f);
            }
            else
            {
              int samplesAvail = abs(B.forwardGoal - mCurrentIndex);
              if (samplesAvail < samplesReq && samplesReq > 0)
              {
                duration *= (float)samplesAvail / (float)samplesReq;
              }
              grain->init(mCurrentIndex, duration, speedG, gain,
                          0.0f);
            }

            // Jitter the grain's render location.
            int jitter = duration * (mGrainJitter.value() * Random::generateFloat(0.0f, 1.0f));
            grain->setDelay(jitter);
            grain->mPhase = mPhase;

            // Save this sample location so that we can detect a stopped condition.
            mLastGrainIndex = mCurrentIndex;
          }
        }
      }

      increment(B, computeSum(mSpeedInput.buffer()));
      mFramesUntilNextGrain--;
    }
  }

  void GrainStretch::processAnyMono(Behavior &B)
  {
    produceGrain(B, true);

    mActiveMonoGrains.clear();
    for (MonoGrain &grain : mMonoGrains)
    {
      if (grain.mActive)
      {
        mActiveMonoGrains.push_back(&grain);
      }
    }

    // sort by sample position for cache coherence
    std::sort(mActiveMonoGrains.begin(), mActiveMonoGrains.end());

    float *left = mLeftOutput.buffer();
    switch (mpSample->mChannelCount)
    {
    case 1:
      if (mOutputChannelCount == 2)
      {
        float *right = mRightOutput.buffer();
        for (MonoGrain *grain : mActiveMonoGrains)
        {
          grain->synthesizeFromMonoToStereo(left, right);
          if (!grain->mActive)
          {
            mFreeMonoGrains.push_back(grain);
          }
        }
      }
      else
      {
        for (MonoGrain *grain : mActiveMonoGrains)
        {
          grain->synthesizeFromMonoToMono(left);
          if (!grain->mActive)
          {
            mFreeMonoGrains.push_back(grain);
          }
        }
      }
      break;
    case 2:
      for (MonoGrain *grain : mActiveMonoGrains)
      {
        grain->synthesizeFromStereo(left);
        if (!grain->mActive)
        {
          mFreeMonoGrains.push_back(grain);
        }
      }
      break;
    }
  }

  void GrainStretch::processStereoToStereo(Behavior &B)
  {
    produceGrain(B, false);

    mActiveStereoGrains.clear();
    for (StereoGrain &grain : mStereoGrains)
    {
      if (grain.mActive)
      {
        mActiveStereoGrains.push_back(&grain);
      }
    }

    // sort by sample position for cache coherence
    std::sort(mActiveStereoGrains.begin(), mActiveStereoGrains.end());

    float *left = mLeftOutput.buffer();
    float *right = mRightOutput.buffer();
    for (StereoGrain *grain : mActiveStereoGrains)
    {
      grain->synthesize(left, right);
      if (!grain->mActive)
      {
        mFreeStereoGrains.push_back(grain);
      }
    }
  }

} /* namespace od */
