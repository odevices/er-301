#pragma once

#include <od/objects/Object.h>
#include <od/extras/RealFFT.h>
#include <od/audio/SampleFifo.h>
#include <vector>

namespace od
{

  class OverlapAddTest : public Object
  {
  public:
    OverlapAddTest();
    virtual ~OverlapAddTest();

#ifndef SWIGLUA
    virtual void process();
    Inlet mInput{"In"};
    Outlet mOutput{"Out"};
    Parameter mHop{"Hop", 1};
#endif

  private:
    SampleFifo mFifo;
    RealFFT mFFT;
    std::vector<float> mSpectrum;
  };

} /* namespace od */
