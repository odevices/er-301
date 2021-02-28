#pragma once

#include <od/objects/Object.h>
#include <od/audio/SampleFifo.h>
#include <core/objects/granular/MonoGrain.h>
#include <array>

namespace od
{

#define MONOPSD_GRAIN_COUNT 3

  class MonoGrainDelay : public Object
  {
  public:
    MonoGrainDelay(float secs);
    virtual ~MonoGrainDelay();

#ifndef SWIGLUA
    virtual void process();
    Inlet mInput{"In"};
    Inlet mDelay{"Delay"};
    Inlet mSpeed{"Speed"};
    Outlet mOutput{"Out"};
#endif

    void setMaxDelay(float secs);

  private:
    SampleFifo mSampleFifo;
    std::array<MonoGrain, MONOPSD_GRAIN_COUNT> mGrains;

    int mSamplesUntilNextOnset = 0;
    float mMaxDelayInSeconds = 0.0f;
    int mMaxDelayInSamples = 0;
    int mGrainDurationInSamples = 0;
    int mGrainPeriodInSamples = 0;

    MonoGrain *getFreeGrain();
  };

} /* namespace od */
