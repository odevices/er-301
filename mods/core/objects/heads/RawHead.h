#pragma once

#include <od/extras/LinearRamp.h>
#include <od/objects/heads/SliceHead.h>

namespace od
{

  class RawHead : public SliceHead
  {
  public:
    RawHead(int channelCount);
    virtual ~RawHead();

    virtual void setSample(Sample *sample, Slices *slices);

#ifndef SWIGLUA
    virtual void process();

    Inlet mTriggerInput{"Trigger"};
    Outlet mLeftOutput{"Left Out"};
    Outlet mRightOutput{"Right Out"};
    Option mRouting{"Routing", CHOICE_LEFT};
    Parameter mSpeed{"Speed", 1};
#endif

  private:
    typedef SliceHead Base;
    int mOutputChannelCount;
    LinearRamp mFade;
    float mLeftBias = 0.0f;
    float mRightBias = 0.0f;

    // optimized for mono
    inline void jumpToMono(int next);
    inline void pushMono(int k, int forwardGoal, int forwardJump,
                         int reverseGoal, int reverseJump);
    void synthesizeMono(Behavior &B0, Behavior &B1, int ti, int step,
                        float *out);

    // optimized for stereo
    inline void jumpToStereo(int next);
    inline void pushStereo(int k, int forwardGoal, int forwardJump,
                           int reverseGoal, int reverseJump);
    void synthesizeStereo(Behavior &B0, Behavior &B1, int ti, int step,
                          float *left, float *right);
  };

} /* namespace od */
