#pragma once

#include <od/objects/Object.h>

namespace od
{

  class SnapToZero : public Object
  {
  public:
    SnapToZero();
    virtual ~SnapToZero();

#ifndef SWIGLUA
    virtual void process();
    Inlet mInput{"In"};
    Outlet mOutput{"Out"};
#endif

    void setThreshold(float threshold);
    void setThresholdInDecibels(float decibels);

  private:
    float mThreshold = 0.0f;
  };

} /* namespace od */
