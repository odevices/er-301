#pragma once

#include <od/objects/Object.h>
#include <vector>

namespace od
{

  class PitchCircle;
  class ScaleQuantizer : public Object
  {
  public:
    ScaleQuantizer();
    virtual ~ScaleQuantizer();

#ifndef SWIGLUA
    virtual void process();

    Inlet mInput{"In"};
    Outlet mOutput{"Out"};
    Parameter mPreTranspose{"Pre-Transpose", 0.0f};   // in cents
    Parameter mPostTranspose{"Post-Transpose", 0.0f}; // in cents
#endif

    void beginScale();
    void addPitch(float cents);
    void endScale();

    bool loadScalaFile(const char *filename);

  protected:
    friend PitchCircle;
    bool mQuantizeAtSampleRate = false;
    std::vector<float> mPitchClasses; // in Volts/octave
    std::vector<float> mPitchBoundaries;
    float mLastInput = 0.0f;
    float mLastOutput = 0.0f;
    bool mHaveScale = false;

    inline float quantize(float value);
  };

} /* namespace od */
