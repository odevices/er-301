#pragma once

#include <od/objects/Object.h>

namespace od
{

  class GainBias : public Object
  {
  public:
    GainBias();
    virtual ~GainBias();

#ifndef SWIGLUA
    virtual void process();
    Inlet mInput{"In"};
    Outlet mOutput{"Out"};
    Parameter mGain{"Gain"};
    Parameter mBias{"Bias"};
#endif

  private:
    float mPreviousGain = 0.0f;
    float mPreviousBias = 0.0f;
  };

} /* namespace od */
