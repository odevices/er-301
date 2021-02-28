#pragma once

#include <od/objects/measurement/Probe.h>
#include <od/extras/RunningRMS.h>
#include <od/extras/FastEWMA.h>

namespace od
{

  class LoudnessProbe : public Probe
  {
  public:
    LoudnessProbe();
    virtual ~LoudnessProbe();

#ifndef SWIGLUA
    virtual void process();

    float value()
    {
      return mRunningRMS.mRMS;
    }

    void reset()
    {
      mRunningRMS.reset();
      mFastEWMA.setInitialState(0);
    }
#endif

    void setPeriod(float secs)
    {
      mRunningRMS.setPeriod(secs);
    }

  protected:
    RunningRMS mRunningRMS;
    FastEWMA mFastEWMA;
  };

} /* namespace od */
