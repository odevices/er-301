#pragma once

#include <od/objects/Object.h>

namespace od
{

  class Stress : public Object
  {
  public:
    Stress();
    virtual ~Stress();

#ifndef SWIGLUA
    virtual void process();
    Inlet mInput{"In"};
    Outlet mOutput{"Out"};
    Parameter mLoad{"Load"};
#endif

    void setAdjustment(float x)
    {
      mAdjustment = x;
    }

  private:
    float mAdjustment = 1.0f;
  };

} /* namespace od */
