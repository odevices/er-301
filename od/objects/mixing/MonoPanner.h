#pragma once

#include <od/objects/Object.h>

namespace od
{

  class MonoPanner : public Object
  {
  public:
    MonoPanner();
    virtual ~MonoPanner();

#ifndef SWIGLUA
    virtual void process();

    Inlet mInput{"In"};
    Inlet mPanInput{"Pan"}; // 0 is left, 1 is right, 0.5 is center

    Outlet mLeftOutput{"Left"};
    Outlet mRightOutput{"Right"};
#endif
  };

} /* namespace od */
