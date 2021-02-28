#pragma once

#include <od/objects/Object.h>

namespace od
{

  class Spread : public Object
  {
  public:
    Spread();
    virtual ~Spread();

#ifndef SWIGLUA
    virtual void process();
    Inlet mInput{"In"};
    Outlet mLeftOutput{"Left Out"};
    Outlet mRightOutput{"Right Out"};
    Inlet mSpread{"Spread"};
#endif
  };

} /* namespace od */
