#pragma once

#include <od/objects/Object.h>
#include <hal/simd.h>

namespace od
{

  class StereoFixedHPF : public Object
  {
  public:
    StereoFixedHPF();
    virtual ~StereoFixedHPF();

#ifndef SWIGLUA
    virtual void process();
    Inlet mLeftInput{"Left In"};
    Inlet mRightInput{"Right In"};
    Outlet mLeftOutput{"Left Out"};
    Outlet mRightOutput{"Right Out"};
    Parameter mCutoff{"Cutoff", 27.5};
#endif

  private:
    float32x2_t mY, mPrevX;
  };

} /* namespace od */
