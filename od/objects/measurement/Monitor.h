#pragma once

#include <od/objects/Object.h>

namespace od
{

  class Monitor : public Object
  {
  public:
    Monitor();
    virtual ~Monitor();

#ifndef SWIGLUA
    virtual void process();

    Inlet mInput{"In"};
    Outlet mOutput{"Out"};
#endif
  };

} /* namespace od */
