#include <od/ui/LinearDialMap.h>
#include <od/extras/Utils.h>
#include <hal/ops.h>

namespace od
{

  LinearDialMap::LinearDialMap(float min, float max) : mMinimum(min), mMaximum(max)
  {
    setRadix(100, 10, 10);
  }

  LinearDialMap::~LinearDialMap()
  {
  }

  void LinearDialMap::setSteps(float superCoarse, float coarse, float fine,
                               float superFine)
  {
    float d = mMaximum - mMinimum;
    int coarseRadix = fastRound(d / coarse);
    int fineRadix = fastRound(coarse / fine);
    int superFineRadix = fastRound(fine / superFine);
    setRadix(coarseRadix, fineRadix, superFineRadix);
    setSuperCoarseMultiplier(fastRound(superCoarse / coarse));
    setRounding(MAX(0.0001f, superFine));
    commitChanges();
  }

  float LinearDialMap::superCoarseStep()
  {
    return mSuperCoarseMultiplier * mCoarseWeight;
  }

  float LinearDialMap::coarseStep()
  {
    return mCoarseWeight;
  }

  float LinearDialMap::fineStep()
  {
    return mFineWeight;
  }

  float LinearDialMap::superFineStep()
  {
    return mSuperFineWeight;
  }

  void LinearDialMap::commitChanges()
  {
    if (mChanged)
    {
      getRadixWeights(mCoarseWeight, mFineWeight, mSuperFineWeight,
                      mMaximum - mMinimum);
      mChanged = false;
    }
  }

  float LinearDialMap::getValue(const DialPosition &dp)
  {
    commitChanges();
    float result = mMinimum + dp.mCoarse * mCoarseWeight + dp.mFine * mFineWeight + dp.mSuperFine * mSuperFineWeight;

    // round to 3 decimal places
    return CLAMP(mMinimum, mMaximum,
                 fastRound(result * mInverseRounding) * mRounding);
  }

  DialPosition LinearDialMap::getPosition(float value)
  {
    if (value < mMinimum)
    {
      DialPosition dp;
      return dp;
    }
    else if (value > mMaximum)
    {
      DialPosition dp;
      dp.mCoarse = mCoarseRadix;
      return dp;
    }
    else
    {
      DialPosition dp;
      commitChanges();
      float phase = value - mMinimum;
      dp.mCoarse = phase / mCoarseWeight;
      phase -= dp.mCoarse * mCoarseWeight;
      dp.mFine = phase / mFineWeight;
      phase -= dp.mFine * mFineWeight;
      dp.mSuperFine = phase / mSuperFineWeight;
      return dp;
    }
  }

  float LinearDialMap::min(bool skipZero)
  {
    return mMinimum;
  }

  float LinearDialMap::max()
  {
    return mMaximum;
  }

} /* namespace od */
