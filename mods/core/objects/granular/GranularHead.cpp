#include <core/objects/granular/GranularHead.h>
#include <hal/ops.h>
#include <algorithm>
#include <math.h>
#include <string.h>

namespace od
{

  GranularHead::GranularHead(int channelCount, int grainCount) : mOutputChannelCount(channelCount)
  {
    addInput(mSpeed);
    addParameter(mGain);
    addParameter(mPan);
    addParameter(mDuration);
    addParameter(mStart);
    addInput(mTrigger);
    addOutput(mLeftOutput);
    addOutput(mRightOutput);
    addParameter(mSquash);
    setMaximumGrainCount(grainCount);
  }

  GranularHead::~GranularHead()
  {
    setSample(0);
  }

  int GranularHead::getGrainCount()
  {
    if (mpSample)
    {
      return mGainCompensation.size();
    }
    else
    {
      return 0;
    }
  }

  Grain *GranularHead::getGrain(int index)
  {
    if (mpSample == 0)
    {
      return NULL;
    }

    if (index < 0 || index >= (int)mGainCompensation.size())
      return NULL;

    if (mOutputChannelCount == 2 && mpSample->mChannelCount == 2)
    {
      return static_cast<Grain *>(&mStereoGrains[index]);
    }
    else
    {
      return static_cast<Grain *>(&mMonoGrains[index]);
    }
  }

  void GranularHead::stopAllGrains()
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

  void GranularHead::setSample(Sample *sample)
  {
    mEnabled = false;
    stopAllGrains();

    Base::setSample(sample);

    if (mpSample)
    {
      mpStart = mpSample->mpData;
      mSpeedAdjustment = sample->mSampleRate * globalConfig.samplePeriod;

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

  void GranularHead::setMaximumGrainCount(int n)
  {
    mFreeStereoGrains.clear();
    mFreeStereoGrains.reserve(n);
    mStereoGrains.resize(n);
    // push grains in a reverse memory order for better cache perf
    for (auto i = mStereoGrains.rbegin(); i != mStereoGrains.rend(); i++)
    {
      StereoGrain &grain = *i;
      mFreeStereoGrains.push_back(&grain);
    }

    mFreeMonoGrains.clear();
    mFreeMonoGrains.reserve(n);
    mMonoGrains.resize(n);
    // push grains in a reverse memory order for better cache perf
    for (auto i = mMonoGrains.rbegin(); i != mMonoGrains.rend(); i++)
    {
      MonoGrain &grain = *i;
      mFreeMonoGrains.push_back(&grain);
    }

    mGainCompensation.resize(n);
    for (int i = 0; i < n; i++)
    {
      mGainCompensation[i] = 1.0f / sqrtf(n - i);
    }
  }

  StereoGrain *GranularHead::getNextFreeStereoGrain()
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

  MonoGrain *GranularHead::getNextFreeMonoGrain()
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

  void GranularHead::process()
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

    // Update head position...
    // Wrap the start parameter into the interval [-1,1]
    float s = mStart.value();
    int si = (int)s;
    s -= si;
    mCurrentIndex = s * mEndIndex;
    // Shift into [0,1] if necessary
    if (mCurrentIndex < 0)
    {
      mCurrentIndex += mEndIndex;
    }

    if (mOutputChannelCount == 2 && mpSample->mChannelCount == 2)
    {
      processStereoToStereo();
    }
    else
    {
      processAnyMono();
    }
  }

  void GranularHead::processAnyMono()
  {
    float *trig = mTrigger.buffer();
    float *speed = mSpeed.buffer();

    for (int i = 0; i < FRAMELENGTH; i++)
    {
      if (trig[i] > 0.0f)
      {
        MonoGrain *grain = getNextFreeMonoGrain();
        if (grain)
        {
          float g = mGain.value() * mGainCompensation[mFreeMonoGrains.size()];
          float p;
          if (mOutputChannelCount == 2)
          {
            p = MAX(-1.0f, MIN(1.0f, mPan.value()));
          }
          else
          {
            p = 0.0f;
          }
          int d = mDuration.value() * globalConfig.sampleRate;
          if (d < 0)
          {
            grain->init(mCurrentIndex, -d, -1.0f * speed[i], g, p);
          }
          else
          {
            grain->init(mCurrentIndex, d, speed[i], g, p);
          }
          grain->setSquash(mSquash.value());
        }
        // Only try to produce one grain per frame
        break;
      }
    }

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

  void GranularHead::processStereoToStereo()
  {
    float *trig = mTrigger.buffer();
    float *speed = mSpeed.buffer();

    for (int i = 0; i < FRAMELENGTH; i++)
    {
      if (trig[i] > 0.0f)
      {
        StereoGrain *grain = getNextFreeStereoGrain();
        if (grain)
        {
          float g = mGain.value() * mGainCompensation[mFreeStereoGrains.size()];
          float p = MAX(-1.0f, MIN(1.0f, mPan.value()));
          int d = mDuration.value() * globalConfig.sampleRate;
          if (d < 0)
          {
            grain->init(mCurrentIndex, -d, -1.0f * speed[i], g, p);
          }
          else
          {
            grain->init(mCurrentIndex, d, speed[i], g, p);
          }
          grain->setSquash(mSquash.value());
        }
        // Only try to produce one grain per frame
        break;
      }
    }

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
