#pragma once

#include <od/objects/Object.h>
#include <hal/simd.h>

namespace od
{

  class Equalizer3 : public Object
  {
  public:
    Equalizer3();
    virtual ~Equalizer3();

#ifndef SWIGLUA
    virtual void process();
    Inlet mInput{"In"};
    Outlet mOutput{"Out"};

    Inlet mLowGain{"Low Gain"};
    Inlet mMidGain{"Mid Gain"};
    Inlet mHighGain{"High Gain"};

    Parameter mLowFreq{"Low Freq", 880.0f};
    Parameter mHighFreq{"High Freq", 5000.0f};
#endif

  private:
#define EQ3_USE_NEON 1
#if EQ3_USE_NEON
    float32x2_t p0, p1, p2, p3;
#else
    // Filter #1 (Low band)
    float f1p0 = 0.0f; // Poles ...
    float f1p1 = 0.0f;
    float f1p2 = 0.0f;
    float f1p3 = 0.0f;

    // Filter #2 (High band)
    float f2p0 = 0.0f; // Poles ...
    float f2p1 = 0.0f;
    float f2p2 = 0.0f;
    float f2p3 = 0.0f;
#endif

    // Sample history buffer
    float sdm1 = 0.0f; // Sample data minus 1
    float sdm2 = 0.0f; // 2
    float sdm3 = 0.0f; // 3

    float mLastLowFreq = -1;
    float mLastHighFreq = -1;
    float lhf[2];
  };

} /* namespace od */
