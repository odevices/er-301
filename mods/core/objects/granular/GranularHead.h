#pragma once

#include <od/objects/heads/Head.h>
#include <core/objects/granular/Grain.h>
#include <core/objects/granular/MonoGrain.h>
#include <core/objects/granular/StereoGrain.h>
#include <atomic>

namespace od
{

  class GranularHead : public Head
  {
  public:
    GranularHead(int channelCount, int grainCount = 16);
    virtual ~GranularHead();

    virtual void setSample(Sample *sample);

#ifndef SWIGLUA
    virtual void process();
    Outlet mLeftOutput{"Left Out"};
    Outlet mRightOutput{"Right Out"};
    Inlet mTrigger{"Trigger"};
    Inlet mSpeed{"Speed"};
    Parameter mDuration{"Duration"};
    Parameter mStart{"Start"};
    Parameter mGain{"Gain"};
    Parameter mPan{"Pan"};
    Parameter mSquash{"Squash"};

    int getGrainCount();
    Grain *getGrain(int index);

#endif

  protected:
    float *mpStart = 0;
    float mSpeedAdjustment = 1.0f;
    int mOutputChannelCount;

    std::vector<StereoGrain> mStereoGrains;
    std::vector<StereoGrain *> mFreeStereoGrains;
    std::vector<StereoGrain *> mActiveStereoGrains;

    std::vector<MonoGrain> mMonoGrains;
    std::vector<MonoGrain *> mFreeMonoGrains;
    std::vector<MonoGrain *> mActiveMonoGrains;

    // gain compensation (indexed by number of free grains)
    std::vector<float> mGainCompensation;

    StereoGrain *getNextFreeStereoGrain();
    MonoGrain *getNextFreeMonoGrain();
    void setMaximumGrainCount(int n);
    void processAnyMono();
    void processStereoToStereo();
    void stopAllGrains();

  private:
    typedef Head Base;
    std::atomic<bool> mEnabled{false};
  };

} /* namespace od */
