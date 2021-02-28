#pragma once

#include <od/objects/Object.h>

namespace od
{

  class DopplerDelay : public Object
  {
  public:
    DopplerDelay(float secs);
    virtual ~DopplerDelay();

#ifndef SWIGLUA
    virtual void process();
    Inlet mInput{"In"};
    Inlet mFeedback{"Feedback"};
    Inlet mDelay{"Delay"};
    Outlet mOutput{"Out"};
#endif

    // returns actual time allocated
    float allocateTimeUpTo(float seconds);
    void deallocate();
    void zero();
    float maximumDelayTime();

  private:
    float *mpBuffer = 0;
    float *mpRead = 0;
    float *mpWrite = 0;
    float *mpEnd = 0;
    float *mpBegin = 0;
    float mPhase = 0;
    float mPreviousDelay = 0; // in secs
    int mMaxDelayInSamples = 0;
    float mMaxDelayInSeconds = 0;

    float mFifo[3] = {0, 0, 0};
    inline void read();
    inline void read4(float *phaseDelta, float *phase, float *recent0,
                      float *recent1, float *recent2);
    inline void write(float *in, float *out, float *feedback, float *phase,
                      float *recent0, float *recent1, float *recent2);

    inline void jumpTo(float *next)
    {
      mpRead = next;
    }

    bool allocate(int Ns);
  };

} /* namespace od */
