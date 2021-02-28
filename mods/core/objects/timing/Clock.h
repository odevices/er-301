#pragma once

#include <od/objects/Object.h>
#include <od/config.h>
#include <hal/ops.h>

namespace od
{

  class Clock : public Object
  {
  public:
    Clock();
    virtual ~Clock();

#ifndef SWIGLUA
    virtual void process();
    Inlet mSync{"Sync"};
    Outlet mOutput{"Out"};
    Parameter mDivider{"Divider", 1};
    Parameter mMultiplier{"Multiplier", 1};
    Parameter mPulseWidth{"Pulse Width", 0.5};
    Option mRational{"Rational", 1};
#endif

  protected:
    uint32_t mPhase = 0;
    float mCurrentValue = 0.0f;
    int mState = 0;

    virtual float getPeriod() = 0;
  };

  class ClockInSeconds : public Clock
  {
  public:
    ClockInSeconds()
    {
      addParameter(mPeriod);
    }
    virtual ~ClockInSeconds()
    {
    }

#ifndef SWIGLUA
    Parameter mPeriod{"Period", 0.5};
#endif

  protected:
    virtual float getPeriod()
    {
      return mPeriod.value();
    }
  };

  class ClockInBPM : public Clock
  {
  public:
    ClockInBPM()
    {
      addParameter(mTempo);
    }
    virtual ~ClockInBPM()
    {
    }

#ifndef SWIGLUA
    Parameter mTempo{"Tempo", 120};
#endif

  protected:
    virtual float getPeriod()
    {
      return 60.0f / MAX(1, mTempo.value());
    }
  };

  class ClockInHertz : public Clock
  {
  public:
    ClockInHertz()
    {
      addParameter(mFrequency);
    }
    virtual ~ClockInHertz()
    {
    }

#ifndef SWIGLUA
    Parameter mFrequency{"Frequency", 2};
#endif

  protected:
    virtual float getPeriod()
    {
      return 1.0f / MAX(1.0f / 60.0f, mFrequency.value());
    }
  };

} /* namespace od */
