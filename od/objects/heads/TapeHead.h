#pragma once

#include <od/objects/heads/Head.h>

namespace od
{

  class TapeHead : public Head
  {
  public:
    TapeHead();
    virtual ~TapeHead();

    virtual void setSample(Sample *sample);
    void setReset(int s);
    void setResetInSeconds(float seconds);
    void setResetProportional(float value);

    void setResets(int left, int right);
    void setStops(int left, int right);
    void setLoops(int leftStart, int leftEnd, int rightStart, int rightEnd);
    void clearStops();

    void start();
    void stop();
    void toggle();
    void reset()
    {
      mResetRequested = true;
    }

#ifndef SWIGLUA
    // reset position
    int mLeftResetIndex = 0;
    int mRightResetIndex = 0;
    // stop points
    int mRightStopIndex = 0;
    int mLeftStopIndex = 0;
    // loop points
    int mRightLoopStartIndex = 0;
    int mRightLoopEndIndex = 0;
    int mLeftLoopStartIndex = 0;
    int mLeftLoopEndIndex = 0;
    // shadow head position
    int mShadowIndex = 0;
    bool mResetRequested = false;
    bool mPaused = false;
#endif

  private:
    typedef Head Base;
  };

} /* namespace od */
