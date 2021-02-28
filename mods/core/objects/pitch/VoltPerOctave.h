#pragma once

#include <od/objects/Object.h>

namespace od
{

  class VoltPerOctave : public Object
  {
  public:
    VoltPerOctave();
    virtual ~VoltPerOctave();

#ifndef SWIGLUA
    virtual void process();

    Inlet mInput{"In"};
    Outlet mOutput{"Out"};
#endif
  };

} /* namespace od */
