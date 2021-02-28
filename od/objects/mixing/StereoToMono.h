#pragma once

#include <od/objects/Object.h>
#include <od/objects/Option.h>

namespace od
{

  class StereoToMono : public Object
  {
  public:
    StereoToMono();
    virtual ~StereoToMono();

#ifndef SWIGLUA
    virtual void process();
    Inlet mLeftInput{"Left In"};
    Inlet mRightInput{"Right In"};
    Outlet mOutput{"Out"};
    Option mRouting{"Routing", CHOICE_LEFT};
#endif
  };

} /* namespace od */
