#pragma once

#include <od/objects/measurement/Probe.h>
#include <od/extras/Fifo.h>

namespace od
{

  class FifoProbe : public Probe
  {
  public:
    FifoProbe(int depth = 8000, int decimate = 2);
    virtual ~FifoProbe();

#ifndef SWIGLUA
    virtual void process();
#endif

    void setDepth(int n);
    void setDecimation(int d);
    void reset();
    float *get(uint32_t n);
    int getRate();
    uint32_t size();

  protected:
    Fifo<float> mBuffer;
    int mDecimate;
  };

} /* namespace od */
