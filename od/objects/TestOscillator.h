#pragma once

#include <od/objects/Object.h>

namespace od
{

  class TestOscillator : public Object
  {
  public:
    TestOscillator();
    virtual ~TestOscillator();

#ifndef SWIGLUA
    virtual void process();
    Inlet mVoltPerOctave{"V/Oct"};
    Inlet mSync{"Sync"};
    Outlet mOutput{"Out"};
    Inlet mFundamental{"Fundamental"};
    Inlet mPhase{"Phase"};
    Inlet mFeedback{"Feedback"};
    Parameter mInternalPhase{"Internal Phase", 0.0f};
#endif

  protected:
    float mLast[4] = {0, 0, 0, 0};
  };

} /* namespace od */
