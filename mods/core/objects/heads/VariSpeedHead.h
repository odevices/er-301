#pragma once

#include <od/extras/LinearRamp.h>
#include <od/objects/heads/SliceHead.h>

namespace od
{

  class VariSpeedHead : public SliceHead
  {
  public:
    VariSpeedHead(int channelCount);
    virtual ~VariSpeedHead();

    virtual void setSample(Sample *sample, Slices *slices);

#ifndef SWIGLUA
    virtual void process();

    Inlet mTriggerInput{"Trigger"};
    Inlet mSpeedInput{"Speed"};
    Outlet mLeftOutput{"Left Out"};
    Outlet mRightOutput{"Right Out"};
    Option mRouting{"Routing", CHOICE_LEFT};
    Option mInterpolation{"Interpolation", INTERPOLATION_QUADRATIC};
#endif

  private:
    typedef SliceHead Base;
    int mOutputChannelCount;
    float mLeftFifo[3] = {0, 0, 0};
    float mRightFifo[3] = {0, 0, 0};
    LinearRamp mFade;
    float mLeftBias = 0.0f;
    float mRightBias = 0.0f;
    float mPhase = 0.0f;

    inline void interpolate(int quality, float *out, float *recent0,
                            float *recent1, float *recent2, float *phase);

    // optimized for mono
    inline void jumpToMono(int next);
    inline void pushMono(int k, int forwardGoal, int forwardJump,
                         int reverseGoal, int reverseJump);
    void synthesizeMono(Behavior &B0, Behavior &B1, int ti, float *speed,
                        float *out);

    // optimized for stereo
    inline void jumpToStereo(int next);
    inline void pushStereo(int k, int forwardGoal, int forwardJump,
                           int reverseGoal, int reverseJump);
    void synthesizeStereo(Behavior &B0, Behavior &B1, int ti, float *speed,
                          float *left, float *right);
  };

} /* namespace od */
