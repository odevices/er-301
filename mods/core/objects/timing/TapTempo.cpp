#include <core/objects/timing/TapTempo.h>
#include <od/config.h>
#include <hal/ops.h>
#include <hal/simd.h>

#define LOCAL_NEON_ENABLE 1

namespace od
{

  TapTempo::TapTempo()
  {
    addInput(mInput);
    addParameter(mBasePeriod);
    mBasePeriod.enableSerialization();
    addParameter(mBaseFrequency);
    mBaseFrequency.enableSerialization();
    addParameter(mDerivedPeriod);
    addParameter(mDerivedFrequency);
    addParameter(mMultiplier);
    addParameter(mDivider);
    addOption(mRational);
    reset();
  }

  TapTempo::~TapTempo()
  {
  }

  void TapTempo::reset()
  {
    mMeasurementClock = 0;
    setBasePeriod(0.5f);
    mCandidatePeriod = mCurrentPeriod;
    mHysteresis = 0.002f * globalConfig.sampleRate;
  }

  void TapTempo::setBaseTempo(float tempo)
  {
    setBasePeriod(60.0f / tempo);
  }

  void TapTempo::setBasePeriod(float secs)
  {
    mCurrentPeriod = MAX(0.0001f, secs);
    mBasePeriod.softSet(mCurrentPeriod);
    float freq = 1.0f / mCurrentPeriod;
    mBaseFrequency.softSet(freq);
  }

  void TapTempo::updateDerived()
  {
    float periodInSeconds = mCurrentPeriod;
    if (mRational.value())
    {
      periodInSeconds *= MAX(1, mDivider.roundTarget());
      periodInSeconds /= MAX(1, mMultiplier.roundTarget());
    }
    else
    {
      periodInSeconds *= MAX(1, mDivider.target());
      periodInSeconds /= MAX(1, mMultiplier.target());
    }

    mDerivedPeriod.softSet(periodInSeconds);
    float freq = 1.0f / periodInSeconds;
    mDerivedFrequency.softSet(freq);
  }

#if LOCAL_NEON_ENABLE
  // 15k cycles (hmm, maybe not worth it...)
  void TapTempo::process()
  {
    float *tap = mInput.buffer();
    float32x4_t thresh = vdupq_n_f32(0.0f);

    for (int i = 0; i < FRAMELENGTH; i += 4)
    {
      float32x4_t x = vld1q_f32(tap + i);
      uint32_t hi[4];
      vst1q_u32(hi, vcgtq_f32(x, thresh));
      for (int j = 0; j < 4; j++)
      {
        if (hi[j])
        {
          // Clock wrapped or N samples of hysteresis
          if (mMeasurementClock < mPrevMeasurement || mMeasurementClock - mPrevMeasurement > mHysteresis)
          {
            mPrevMeasurement = mMeasurementClock;
          }
          mCandidatePeriod = MAX(0.0001f,
                                 mPrevMeasurement * globalConfig.samplePeriod);
          setBasePeriod(mCandidatePeriod);
          mMeasurementClock = 0;
        }

        mMeasurementClock++;
      }
    }

    updateDerived();
  }
#else
  // 18k cycles
  void TapTempo::process()
  {
    float *tap = mInput.buffer();

    for (uint32_t i = 0; i < globalConfig.frameLength; i++)
    {

      if (tap[i] > 0.0f)
      {
        // 2 samples of hysteresis
        if (abs(mMeasurementClock - mPrevMeasurement) > mHysteresis)
        {
          mPrevMeasurement = mMeasurementClock;
        }
        mCandidatePeriod = MAX(0.0001f,
                               mPrevMeasurement * globalConfig.samplePeriod);
        setBasePeriod(mCandidatePeriod);
        mMeasurementClock = 0;
      }

      mMeasurementClock++;
    }

    updateDerived();
  }
#endif
} // namespace od
/* namespace od */
