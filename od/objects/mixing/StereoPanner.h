#pragma once

#include <od/objects/Object.h>

namespace od
{

  class StereoPanner : public Object
  {
    // or Balance
  public:
    StereoPanner();
    virtual ~StereoPanner();

#ifndef SWIGLUA
    virtual void process();

    Inlet mLeftInput{"Left In"};
    Inlet mRightInput{"Right In"};
    Inlet mPanInput{"Pan"}; // 0 is left, 1 is right, 0.5 is center

    Outlet mLeftOutput{"Left Out"};
    Outlet mRightOutput{"Right Out"};
#endif
  };

} /* namespace od */
