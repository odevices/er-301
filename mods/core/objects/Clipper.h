#pragma once

#include <od/objects/Object.h>

namespace od
{

  class Clipper : public Object
  {
  public:
    Clipper();
    Clipper(float min, float max);
    virtual ~Clipper();

#ifndef SWIGLUA
    virtual void process();
    Inlet mInput{"In"};
    Outlet mOutput{"Out"};
#endif

    void setMinimum(float value)
    {
      mMinimum = value;
    }

    void setMaximum(float value)
    {
      mMaximum = value;
    }

  private:
    float mMinimum = -1.0f;
    float mMaximum = 1.0f;
  };

} /* namespace od */
