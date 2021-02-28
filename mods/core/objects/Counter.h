#pragma once

#include <od/objects/Object.h>

namespace od
{

  class Counter : public Object
  {
  public:
    Counter();
    virtual ~Counter();

#ifndef SWIGLUA
    virtual void process();
    Inlet mInput{"In"};
    Inlet mReset{"Reset"};
    Outlet mOutput{"Out"};
    Parameter mStepSize{"Step Size"};
    Parameter mStart{"Start"};
    Parameter mFinish{"Finish"};
    Parameter mGain{"Gain", 1.0f};
    Parameter mBias{"Bias", 0.0f};
    Parameter mValue{"Value", 0.0f};
    Option mWrap{"Wrap", CHOICE_YES};
    Option mProcessingRate{"Processing Rate", PER_FRAME};
#endif

  protected:
    void atFrameRate();
    void atSampleRate();
  };

} /* namespace od */
