#pragma once

#include <core/objects/granular/MonoGrain.h>
#include <core/objects/granular/StereoGrain.h>
#include <od/objects/heads/SliceHead.h>
#include <atomic>

namespace od
{

  class GrainStretch : public SliceHead
  {
  public:
    GrainStretch(int channelCount, int grainCount);
    virtual ~GrainStretch();

    virtual void setSample(Sample *sample, Slices *slices);

#ifndef SWIGLUA
    virtual void process();
    Inlet mTriggerInput{"Trigger"};
    Inlet mSpeedInput{"Speed"};
    Outlet mLeftOutput{"Left Out"};
    Outlet mRightOutput{"Right Out"};
    Parameter mGrainDuration{"Grain Duration", 0.1f};
    Parameter mGrainPitch{"Grain Pitch", 0.0f};
    Parameter mGrainJitter{"Grain Jitter", 0.0f};
#endif

  private:
    typedef SliceHead Base;
    std::atomic<bool> mEnabled{false};

    std::vector<StereoGrain> mStereoGrains;
    std::vector<StereoGrain *> mFreeStereoGrains;
    std::vector<StereoGrain *> mActiveStereoGrains;

    std::vector<MonoGrain> mMonoGrains;
    std::vector<MonoGrain *> mFreeMonoGrains;
    std::vector<MonoGrain *> mActiveMonoGrains;

    int mOutputChannelCount;
    int mGrainCount;
    int mFramesUntilNextGrain = 0;
    int mLastGrainIndex = -1;
    float mPhase = 0.0f;

    StereoGrain *getNextFreeStereoGrain();
    MonoGrain *getNextFreeMonoGrain();
    void setMaximumGrainCount(int n);
    void processAnyMono(Behavior &B);
    void processStereoToStereo(Behavior &B);
    void stopAllGrains();
    void increment(Behavior &B, float delta);
    void produceGrain(Behavior &B, bool mono);
    void incrementPhase(int k, int forwardGoal, int forwardJump,
                        int reverseGoal, int reverseJump);            
  };

} /* namespace od */
