#pragma once

#include <od/objects/heads/RecordHead.h>
#include <od/extras/LinearRamp.h>

namespace od
{

  class DubLooper : public RecordHead
  {
  public:
    DubLooper(int channelCount);
    virtual ~DubLooper();

#ifndef SWIGLUA
    virtual void process();
    Inlet mReset{"Reset"};
    Inlet mPunch{"Punch"};
    Inlet mEngage{"Engage"};
    // recommended to put a (0,1) limiter on this input
    Inlet mDub{"Dub"};
    Parameter mResetPosition{"Start"};
    Parameter mFadeTime{"Fade Time", 0.005f};
    Inlet mLeftInput{"Left In"};
    Inlet mRightInput{"Right In"};
    Outlet mLeftOutput{"Left Out"};
    Outlet mRightOutput{"Right Out"};
#endif

    void setSample(Sample *sample);

  private:
    int mChannelCount;

    // pop prevention
    LinearRamp mPunchFade;
    bool mPunched = false;
    LinearRamp mShadowFade;
    int mShadowIndex = 0;

    inline void jumpTo(int next, int fadeTime)
    {
      mShadowFade.setLength(fadeTime);
      mShadowIndex = mCurrentIndex;
      mCurrentIndex = next;
      mShadowFade.reset(0.0f, 1.0f);
    }

    void mono();
    void stereo();
  };
} /* namespace od */
