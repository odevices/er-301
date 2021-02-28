#pragma once

#include <od/objects/Object.h>

namespace od
{

#define ALLPASSFILTER_MAX_ORDER 24

  class AllPassFilter : public Object
  {
  public:
    AllPassFilter();
    virtual ~AllPassFilter();

#ifndef SWIGLUA
    virtual void process();
    Inlet mInput{"In"};
    Inlet mFrequency{"Frequency"};
    Inlet mFeedback{"Feedback"};
    Outlet mOutput{"Out"};
    Parameter mOrder{"Order"};
#endif

  private:
    float mYState[ALLPASSFILTER_MAX_ORDER];
    float mXState[ALLPASSFILTER_MAX_ORDER];
  };

} /* namespace od */
