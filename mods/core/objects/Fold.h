#pragma once

#include <od/objects/Object.h>

namespace od
{

  class Fold : public Object
  {
  public:
    Fold();
    virtual ~Fold();

#ifndef SWIGLUA
    virtual void process();
    Inlet mInput{"In"};
    Outlet mOutput{"Out"};
    Inlet mThreshold{"Threshold"};
    Inlet mUpperGain{"Upper Gain"};
    Inlet mLowerGain{"Lower Gain"};
#endif
  };

} /* namespace od */
