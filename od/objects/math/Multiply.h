#pragma once
#include <od/objects/Object.h>

namespace od
{

  class Multiply : public Object
  {
  public:
    Multiply();
    virtual ~Multiply();

#ifndef SWIGLUA
    virtual void process();
    Inlet mLeftInput{"Left"};
    Inlet mRightInput{"Right"};
    Outlet mOutput{"Out"};
#endif
  };

} /* namespace od */
