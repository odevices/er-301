#pragma once

#include <od/objects/Object.h>

namespace od
{

  class VelvetNoise : public Object
  {
  public:
    VelvetNoise();
    virtual ~VelvetNoise();

#ifndef SWIGLUA
    virtual void process();
    Outlet mOutput{"Out"};
    Parameter mRate{"Rate", 1000.0f};
#endif

  private:
    int mPhase = 0;
    int mGoal = 0;
  };

} /* namespace od */