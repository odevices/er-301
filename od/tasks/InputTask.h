#pragma once

#include <od/tasks/Task.h>
#include <od/objects/Inlet.h>
#include <od/objects/Outlet.h>
#include <hal/constants.h>

namespace od
{

  class InputTask : public Task
  {
  public:
    InputTask();
    virtual ~InputTask();

    virtual void process(float *inputs, float *outputs);

    Outlet mA1{"A1"};
    Outlet mA2{"A2"};
    Outlet mA3{"A3"};

    Outlet mB1{"B1"};
    Outlet mB2{"B2"};
    Outlet mB3{"B3"};

    Outlet mC1{"C1"};
    Outlet mC2{"C2"};
    Outlet mC3{"C3"};

    Outlet mD1{"D1"};
    Outlet mD2{"D2"};
    Outlet mD3{"D3"};

    Outlet mG1{"G1"};
    Outlet mG2{"G2"};
    Outlet mG3{"G3"};
    Outlet mG4{"G4"};

    Outlet mIN1{"IN1"};
    Outlet mIN2{"IN2"};
    Outlet mIN3{"IN3"};
    Outlet mIN4{"IN4"};

    float mLastInput[NUM_INPUT_CHANNELS];
  };

} /* namespace od */
