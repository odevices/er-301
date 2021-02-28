#pragma once
#include <od/objects/Object.h>

namespace od
{

  class Probe : public Object
  {
  public:
    Probe();
    virtual ~Probe();

#ifndef SWIGLUA
    Inlet mInput{"In"};
#endif
  };

} /* namespace od */
