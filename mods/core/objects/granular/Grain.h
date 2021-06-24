#pragma once

#include <od/audio/Sample.h>

namespace od
{

  class Grain
  {
  public:
    Grain();
    virtual ~Grain();

    void setSample(Sample *sample);
    void init(int start, int duration, float speed, float gain, float pan);
    void snapToZeroCrossing(int window);
    void stop();

    // envelope types
    static const int mSineWindow = 0;
    static const int mHanningWindow = 1;
    static const int mTrapezoidWindow = 2;

    void setEnvelope(int type);
    void setFade(int fade);
    void setSquash(float squash);
    void setDelay(int samples);
    int samplesRequired();

    inline bool operator<(const Grain &x) const
    {
      return (mCurrentIndex < x.mCurrentIndex);
    }

    Sample *mpSample = 0;
    float mEnvelopePhase = 0.0f;
    float mEnvelopePhaseDelta = 0.0f;
    int mEnvelopeType = mSineWindow;
    int mFade = 0; // in samples
    int mCurrentIndex = 0;
    int mDuration = 0;
    int mRemaining = 0;
    int mDelayInSamples = 0;
    float mSquash = 0.0f;

    float mSpeedAdjustment = 1.0f;
    float mPhaseDelta = 0.0f;
    float mPhase = 0.0f;
    float mLeftBalance = 1.0f;
    float mRightBalance = 1.0f;
    float mLastEnvelopeValue = 0.0f;

    bool mActive = false;

  protected:
    void generateSineWindow(float *out, int n);
    void generateHanningWindow(float *out, int n);
    void generateTrapezoidWindow(float *out, int n);
    void generateEnvelope(float *out, int n);
    void squashEnvelope(float *out, int n);
  };

} /* namespace od */
