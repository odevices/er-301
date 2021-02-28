#pragma once

#include <od/objects/Object.h>

namespace od
{

  class CrossFade : public Object
  {
  public:
    CrossFade();
    virtual ~CrossFade();

#ifndef SWIGLUA
    virtual void process();
    Inlet mInputA{"A"};
    Inlet mInputB{"B"};
    Inlet mFade{"Fade"};
    Outlet mOutput{"Out"};
#endif
  };

} /* namespace od */
