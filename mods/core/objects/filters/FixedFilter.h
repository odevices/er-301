#pragma once

#include <od/objects/Object.h>
#include <core/extras/BiquadFilter.h>

namespace od
{

  class FixedFilter : public Object
  {
  public:
    FixedFilter(int order);
    virtual ~FixedFilter();

#ifndef SWIGLUA
    virtual void process();
    Inlet mInput{"In"};
    Outlet mOutput{"Out"};
#endif

    void setLowPass(float cutoff, float Q = BIQUAD_FLAT_Q)
    {
      for (int i = 0; i < mOrder; i++)
      {
        mpBiquadFilters[i].designLowPass(cutoff, Q);
        mpBiquadFilters[i].initState(0.0f);
      }
    }

    void setHighPass(float cutoff, float Q = BIQUAD_FLAT_Q)
    {
      for (int i = 0; i < mOrder; i++)
      {
        mpBiquadFilters[i].designHighPass(cutoff, Q);
        mpBiquadFilters[i].initState(0.0f);
      }
    }

    void setBandPass(float cutoff, float Q = BIQUAD_FLAT_Q)
    {
      for (int i = 0; i < mOrder; i++)
      {
        mpBiquadFilters[i].designBandPass(cutoff, Q);
        mpBiquadFilters[i].initState(0.0f);
      }
    }

  private:
    int mOrder = 0;
    BiquadFilter *mpBiquadFilters = 0;
  };

} /* namespace od */
