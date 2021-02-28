#pragma once

#include <od/objects/Object.h>
#include <limits>

namespace od
{

  class ConstantGain : public Object
  {
  public:
    ConstantGain();
    virtual ~ConstantGain();

#ifndef SWIGLUA
    virtual void process();
    Inlet mInput{"In"};
    Outlet mOutput{"Out"};
    Parameter mGain{"Gain"};
#endif

    void setClamp(float threshold);
    void setClampInDecibels(float threshold);

  private:
    float mClamp = std::numeric_limits<float>::min();
    float mPreviousGain = 0.0f;
  };

} /* namespace od */
