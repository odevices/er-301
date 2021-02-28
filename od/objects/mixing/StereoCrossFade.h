#pragma once

#include <od/objects/Object.h>

namespace od
{

  class StereoCrossFade : public Object
  {
  public:
    StereoCrossFade();
    virtual ~StereoCrossFade();

#ifndef SWIGLUA
    virtual void process();

    Inlet mLeftInputA{"Left A"};
    Inlet mLeftInputB{"Left B"};
    Inlet mRightInputA{"Right A"};
    Inlet mRightInputB{"Right B"};
    Inlet mFade{"Fade"};
    Outlet mLeftOutput{"Left Out"};
    Outlet mRightOutput{"Right Out"};
#endif
  };

} /* namespace od */
