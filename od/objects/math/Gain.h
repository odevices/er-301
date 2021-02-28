#pragma once

#include <od/objects/Object.h>

namespace od
{

  class Gain : public Object
  {
  public:
    Gain();
    virtual ~Gain();

#ifndef SWIGLUA
    virtual void process();
    Inlet mInput{"In"};
    Inlet mGain{"Gain"};
    Outlet mOutput{"Out"};
    Parameter mBaseGain{"BaseGain"};
#endif
  };

} /* namespace od */
