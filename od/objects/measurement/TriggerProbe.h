#pragma once

#include <od/objects/measurement/Probe.h>

namespace od
{

  class TriggerProbe : public Probe
  {
  public:
    TriggerProbe();
    virtual ~TriggerProbe();

#ifndef SWIGLUA
    virtual void process();
    Option mState{"State", 0};
    Parameter mThreshold{"Threshold", 0.1f};
#endif

    void reset();

  protected:
    bool mManualReset = true;
  };

} /* namespace od */
