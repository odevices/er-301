#pragma once

#include <od/extras/LinearRamp.h>
#include <od/objects/heads/TapeHead.h>

namespace od
{

  class LoopHead : public TapeHead
  {
  public:
    LoopHead(int channelCount);
    virtual ~LoopHead();
    virtual void setSample(Sample *sample);

#ifndef SWIGLUA
    virtual void process();
    Inlet mSyncInput{"Sync"};
    Inlet mSpeedInput{"Speed"};
    Outlet mLeftOutput{"Left Out"};
    Outlet mRightOutput{"Right Out"};
    Parameter mLoopStart{"Loop Start", 0.0f};
    Parameter mLoopLength{"Loop Length", 1.0f};
    Parameter mFadeTime{"Fade Time", 0.01f};
    Parameter mMaxLoopDuration{"Maximum Loop Duration", 5.0f};
    Parameter mActualLoopDuration{"Actual Loop Duration", 0.0f}; // output
    Option mInterpolation{"Interpolation", INTERPOLATION_QUADRATIC};
    Option mRouting{"Routing", CHOICE_LEFT};
#endif

  private:
    typedef TapeHead Base;
    int mOutputChannelCount;
    float mPhase = 0.0f;
    float mSpeedAdjustment = 1.0f;
    float mLeftFifo[3] = {0, 0, 0};
    float mRightFifo[3] = {0, 0, 0};
    LinearRamp mFade;
    float mLeftBias = 0.0f;
    float mRightBias = 0.0f;
    int mSavedLoopStart = 0;
    int mSavedLoopEnd = 0;

    inline void jumpToStereo(int next);
    inline void jumpToMono(int next);

    inline void pushStereo(int k, int loopStart, int loopEnd);
    inline void pushMono(int k, int loopStart, int loopEnd);

    void synthesizeMono(int loopStart, int loopEnd, int ti, float *speed, float *out);
    void synthesizeStereo(int loopStart, int loopEnd, int ti, float *speed, float *left, float *right);
  };

} /* namespace od */
