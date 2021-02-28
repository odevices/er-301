#pragma once

#include <od/objects/Object.h>

namespace od
{

  class WhiteNoise : public Object
  {
  public:
    WhiteNoise();
    virtual ~WhiteNoise();

#ifndef SWIGLUA
    virtual void process();
    Outlet mOutput{"Out"};
#endif
  };

} /* namespace od */
