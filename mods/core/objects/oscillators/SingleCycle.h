#pragma once

#include <od/objects/heads/SliceHead.h>

namespace od
{

  class SingleCycle : public SliceHead
  {
  public:
    SingleCycle();
    virtual ~SingleCycle();

#ifndef SWIGLUA
    virtual void process();
    Inlet mVoltPerOctave{"V/Oct"};
    Inlet mSync{"Sync"};
    Outlet mOutput{"Out"};
    Inlet mFundamental{"Fundamental"};
    Inlet mPhase{"Phase"};
    Parameter mInternalPhase{"Internal Phase", 0.0f};
#endif

  protected:
    void processOne(int offset, int length);
    void processMany();

  private:
    typedef SliceHead Base;
  };

} /* namespace od */
