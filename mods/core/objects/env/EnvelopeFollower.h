#pragma once

#include <od/objects/Object.h>

namespace od
{

  class EnvelopeFollower : public Object
  {
  public:
    EnvelopeFollower();
    virtual ~EnvelopeFollower();

#ifndef SWIGLUA
    virtual void process();

    Inlet mInput{"In"};
    Outlet mOutput{"Out"};

    Parameter mAttack{"Attack Time", 0.001f};
    Parameter mRelease{"Release Time", 0.010f};

#endif

  private:
    float mEnvelope = 0.0f;
    float mThreshold = 0.0f;
  };

} /* namespace od */
