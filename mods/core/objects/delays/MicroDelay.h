#pragma once

#include <od/objects/Object.h>

namespace od
{

  class MicroDelay : public Object
  {
  public:
    MicroDelay(float secs);
    virtual ~MicroDelay();

#ifndef SWIGLUA
    virtual void process();
    Inlet mInput{"In"};
    Parameter mDelay{"Delay"};
    Outlet mOutput{"Out"};
#endif

  private:
    std::vector<float> mBuffer;
    int mReadIndex = 0;
    int mWriteIndex = 0;
    int mMaxDelayInSamples = 0;

    void setMaxDelay(int n);
  };

} /* namespace od */
