#pragma once

#include <od/objects/Object.h>
#include <limits>

namespace od
{

  class Constant : public Object
  {
  public:
    Constant();
    virtual ~Constant();

#ifndef SWIGLUA
    virtual void process();
    Outlet mOutput{"Out"};
    Parameter mValue{"Value"};
#endif

    void setClamp(float threshold);
    void setClampInDecibels(float threshold);

  private:
    float mClamp = std::numeric_limits<float>::min();
    float mPreviousValue = 0.0f;
  };

} /* namespace od */
