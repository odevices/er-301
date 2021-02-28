#pragma once

#include <od/objects/Object.h>
#include <od/extras/LinearRamp.h>

namespace od
{

  class Delay : public Object
  {
  public:
    Delay(int channelCount);
    virtual ~Delay();

#ifndef SWIGLUA
    virtual void process();
    Inlet mLeftInput{"Left In"};
    Inlet mRightInput{"Right In"};
    Inlet mFeedback{"Feedback"};
    Parameter mLeftDelay{"Left Delay"};
    Parameter mRightDelay{"Right Delay"};
    Parameter mSpread{"Spread", 0};
    Outlet mLeftOutput{"Left Out"};
    Outlet mRightOutput{"Right Out"};
#endif

    // returns actual time allocated
    float allocateTimeUpTo(float seconds);
    void deallocate();
    void zero();
    float maximumDelayTime();

  private:
    struct InternalDelayLine
    {
      char *mpBuffer = 0;
      int mReadIndex0 = 0;
      int mReadIndex1 = 0;
      int mWriteIndex = 0;
    };

    InternalDelayLine mLeftDelayLine;
    InternalDelayLine mRightDelayLine;

    int mChannelCount;
    int mMaxDelayInSamples = 0;
    LinearRamp mFade;

    bool allocate(int Ns);
    void updateTime(InternalDelayLine &D, float delay);
    void pushSamples(InternalDelayLine &D, float *in, float *out,
                     float *feedback, float *fade);
  };

} /* namespace od */
