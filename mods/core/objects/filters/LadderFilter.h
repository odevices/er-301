#pragma once

#include <od/objects/Object.h>

namespace od
{

  class LadderFilter : public Object
  {
  public:
    LadderFilter();
    virtual ~LadderFilter();

#ifndef SWIGLUA
    virtual void process();
    Inlet mInput{"In"};
    Inlet mVoltPerOctave{"V/Oct"};
    Inlet mResonance{"Resonance"};
    Outlet mOutput{"Out"};
    Parameter mFundamental{"Fundamental", 880.0f};
#endif

  private:
    float mStage[4];
    float mDelay[4];
  };

} /* namespace od */
