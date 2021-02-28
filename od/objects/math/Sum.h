#pragma once

#include <od/objects/Object.h>

namespace od
{

  class Sum : public Object
  {
  public:
    Sum();
    virtual ~Sum();

#ifndef SWIGLUA
    virtual void process();

    Inlet mLeftInput{"Left"};
    Inlet mRightInput{"Right"};
    Outlet mOutput{"Out"};
#endif
  };

} /* namespace od */
