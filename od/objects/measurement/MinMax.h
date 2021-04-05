#pragma once
#include <od/objects/Object.h>

namespace od
{

  class MinMax : public Object
  {
  public:
    MinMax();
    virtual ~MinMax();

#ifndef SWIGLUA
    virtual void process();
    Inlet mInput{"In"};
    Parameter mMinimum{"Min"};
    Parameter mMaximum{"Max"};
    Parameter mCenter{"Center"};
#endif

  private:
    int mCount = 0;
    int mPeriod = 0;
    float mNextMin = 0;
    float mNextMax = 0;
  };

} /* namespace od */
