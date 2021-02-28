#pragma once

#include <od/objects/Object.h>
#include <hal/simd.h>

namespace od
{

  class StereoLadderFilter : public Object
  {
  public:
    StereoLadderFilter();
    virtual ~StereoLadderFilter();

#ifndef SWIGLUA
    virtual void process();
    Inlet mLeftInput{"Left In"};
    Inlet mRightInput{"Right In"};
    Inlet mVoltPerOctave{"V/Oct"};
    Inlet mResonance{"Resonance"};
    Inlet mFundamental{"Fundamental"};
    Outlet mLeftOutput{"Left Out"};
    Outlet mRightOutput{"Right Out"};
#endif

  private:
    float32x2_t mStage0, mStage1, mStage2, mStage3;
    float32x2_t mDelay0, mDelay1, mDelay2, mDelay3;
  };

} /* namespace od */
