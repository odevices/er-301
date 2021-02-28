#pragma once

#include <od/objects/Object.h>

namespace od
{

  class RationalMultiply : public Object
  {
  public:
    RationalMultiply(bool quantize = false);
    virtual ~RationalMultiply();

#ifndef SWIGLUA
    virtual void process();
    Inlet mInput{"In"};
    Inlet mNumerator{"Numerator"};
    Inlet mDivisor{"Divisor"};
    Outlet mOutput{"Out"};
#endif

  private:
    bool mQuantize;
  };

} /* namespace od */
