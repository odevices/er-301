#pragma once

#include <od/objects/Object.h>

namespace od
{

  class SawtoothOscillator : public Object
  {
  public:
    SawtoothOscillator();
    virtual ~SawtoothOscillator();
#ifndef SWIGLUA
    virtual void process();
    Inlet mVoltPerOctave{"V/Oct"};
    Inlet mSync{"Sync"};
    Outlet mOutput{"Out"};
    Inlet mFundamental{"Fundamental"};
    Parameter mPhase{"Phase", 0.0f};
#endif
  };

} /* namespace od */
