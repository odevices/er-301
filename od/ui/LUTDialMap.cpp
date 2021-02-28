#include <od/ui/LUTDialMap.h>
#include <od/extras/Utils.h>
#include <hal/ops.h>
#include <algorithm>

namespace od
{

  LUTDialMap::LUTDialMap(int size, int coarseMultiple, int fineRadix,
                         int superFineRadix)
  {
    size = MAX(2, size);
    mValues.reserve(size);
    mCount = size;
    setRadix((size - 1) * coarseMultiple, fineRadix, superFineRadix);
  }

  LUTDialMap::~LUTDialMap()
  {
  }

  void LUTDialMap::commitChanges()
  {
    getRadixWeights(mCoarseWeight, mFineWeight, mSuperFineWeight, mCount - 1);
  }

  void LUTDialMap::add(float value)
  {
    mValues.push_back(value);
    markChanged();
  }

  float LUTDialMap::getValue(const DialPosition &dp)
  {
    if (mValues.size() < 2)
    {
      return 0.0f;
    }

    commitChanges();
    float phase = dp.mCoarse * mCoarseWeight + dp.mFine * mFineWeight + dp.mSuperFine * mSuperFineWeight;

    float result;

    if (mInterpolated)
    {
      int N = mValues.size() - 1;
      int i = CLAMP(0, N, (int)phase);
      int j = MIN(N, i + 1);
      if (i == j)
      {
        return mValues[i];
      }
      float x = (float)i;
      float w1 = phase - x;
      float w0 = 1.0f - w1;
      result = w0 * mValues[i] + w1 * mValues[j];
    }
    else
    {
      result = mValues[(int)phase];
    }

    // round to 3 decimal places
    return MIN(max(), fastRound(result * mInverseRounding) * mRounding);
  }

  DialPosition LUTDialMap::getPosition(float value)
  {
    auto upper = std::upper_bound(mValues.begin(), mValues.end(), value);
    if (upper == mValues.begin())
    {
      DialPosition dp;
      return dp;
    }
    float a = *(upper - 1);
    float b = *upper;
    float phase = (float)(upper - 1 - mValues.begin());
    float d = b - a;
    if (mInterpolated && d > 1e-8)
    {
      phase += (value - a) / d;
    }

    DialPosition dp;
    commitChanges();
    dp.mCoarse = phase / mCoarseWeight;
    phase -= dp.mCoarse * mCoarseWeight;
    dp.mFine = phase / mFineWeight;
    phase -= dp.mFine * mFineWeight;
    dp.mSuperFine = phase / mSuperFineWeight;
    return dp;
  }

  float LUTDialMap::min(bool skipZero)
  {
    if (skipZero)
    {
      for (float x : mValues)
      {
        if (x > 0.0f)
        {
          return x;
        }
      }
      return max();
    }
    else
    {
      return mValues.front();
    }
  }

  float LUTDialMap::max()
  {
    return mValues.back();
  }

} /* namespace od */
