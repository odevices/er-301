#pragma once

#include <od/objects/measurement/Probe.h>

namespace od
{

  class CaptureProbe : public Probe
  {
  public:
    CaptureProbe();
    virtual ~CaptureProbe();

#ifndef SWIGLUA
    virtual void process();
#endif

    void resetCount()
    {
      mCount = 0;
    }

    inline void setBuffer(float *buffer, uint32_t size)
    {
      mpBuffer = buffer;
      mBufferSize = size;
    }

    inline uint32_t count()
    {
      return mCount;
    }

  protected:
    float *mpBuffer = 0;
    uint32_t mBufferSize = 0;
    uint32_t mCount = 0;
  };

} /* namespace od */
