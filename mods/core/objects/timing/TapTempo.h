#pragma once

#include <od/objects/Object.h>

namespace od
{

  class Looper;

  class TapTempo : public Object
  {
  public:
    TapTempo();
    virtual ~TapTempo();

#ifndef SWIGLUA
    virtual void process();
    Inlet mInput{"In"};
    Parameter mBasePeriod{"Base Period"};
    Parameter mBaseFrequency{"Base Frequency"};
    Parameter mDerivedPeriod{"Derived Period"};
    Parameter mDerivedFrequency{"Derived Frequency"};
    Parameter mMultiplier{"Multiplier", 1};
    Parameter mDivider{"Divider", 1};
    Option mRational{"Rational", 1};
#endif

    void reset();
    void setBasePeriod(float secs);
    void setBaseTempo(float bpm);

  protected:
    uint32_t mMeasurementClock = 0;
    uint32_t mPrevMeasurement = 0;
    uint32_t mHysteresis = 2;
    float mCurrentPeriod;
    float mCandidatePeriod;

    void updateDerived();
    friend Looper;
  };

} /* namespace od */
