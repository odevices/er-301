#pragma once

#include <od/objects/Object.h>

#define RECTIFY_POSITIVEHALF 1
#define RECTIFY_NEGATIVEHALF 2
#define RECTIFY_FULL 3

namespace od
{

  class Rectify : public Object
  {
  public:
    Rectify();
    virtual ~Rectify();

#ifndef SWIGLUA
    virtual void process();
    Inlet mInput{"In"};
    Outlet mOutput{"Out"};
    Option mType{"Type", RECTIFY_POSITIVEHALF};
#endif
  };

} /* namespace od */
