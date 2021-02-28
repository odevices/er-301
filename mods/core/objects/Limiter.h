#pragma once

#include <od/objects/Object.h>

#define LIMITER_INVSQRT 1
#define LIMITER_CUBIC 2
#define LIMITER_HARD 3

namespace od
{

  class Limiter : public Object
  {
  public:
    Limiter();
    virtual ~Limiter();

#ifndef SWIGLUA
    virtual void process();
    Inlet mInput{"In"};
    Outlet mOutput{"Out"};
    Option mType{"Type", LIMITER_CUBIC};
#endif
  };

} /* namespace od */
