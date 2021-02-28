/*
 * DialMap.cpp
 *
 *  Created on: Oct 9, 2016
 *      Author: clarkson
 */

#include <od/ui/DialMap.h>
#include <hal/ops.h>
#include <od/extras/Utils.h>

namespace od
{

  DialMap::DialMap()
  {
  }

  DialMap::~DialMap()
  {
  }

  void DialMap::setSuperFineRadix(int radix)
  {
    mSuperFineRadix = MAX(1, radix);
    markChanged();
  }

  void DialMap::setFineRadix(int radix)
  {
    mFineRadix = MAX(1, radix);
    markChanged();
  }

  void DialMap::setCoarseRadix(int radix)
  {
    mCoarseRadix = MAX(1, radix);
    markChanged();
  }

  void DialMap::setRadix(int coarse, int fine, int superFine)
  {
    mSuperFineRadix = MAX(1, superFine);
    mFineRadix = MAX(1, fine);
    mCoarseRadix = MAX(1, coarse);
    markChanged();
  }

  bool DialMap::updateSuperFine(DialPosition &dp, int change)
  {
    bool changed = false;

    while (change > 0)
    {
      if (dp.mSuperFine + 1 < mSuperFineRadix)
      {
        dp.mSuperFine++;
        changed = true;
      }
      else if (updateFine(dp, 1))
      {
        dp.mSuperFine = 0;
        changed = true;
      }
      change--;
    }

    while (change < 0)
    {
      if (dp.mSuperFine > 0)
      {
        dp.mSuperFine--;
        changed = true;
      }
      else if (updateFine(dp, -1))
      {
        dp.mSuperFine = mSuperFineRadix - 1;
        changed = true;
      }
      change++;
    }

    return changed;
  }

  bool DialMap::updateFine(DialPosition &dp, int change)
  {
    bool changed = false;

    while (change > 0)
    {
      if (dp.mFine + 1 < mFineRadix)
      {
        dp.mFine++;
        changed = true;
      }
      else if (updateCoarse(dp, 1))
      {
        dp.mFine = 0;
        changed = true;
      }
      change--;
    }

    while (change < 0)
    {
      if (dp.mFine > 0)
      {
        dp.mFine--;
        changed = true;
      }
      else if (updateCoarse(dp, -1))
      {
        dp.mFine = mFineRadix - 1;
        changed = true;
      }
      change++;
    }

    return changed;
  }

  bool DialMap::updateCoarse(DialPosition &dp, int change)
  {
    int saved = dp.mCoarse;
    dp.mCoarse = CLAMP(0, mCoarseRadix, dp.mCoarse + change);
    return saved != dp.mCoarse;
  }

  DialPosition DialMap::validatePosition(const DialPosition &dp)
  {
    DialPosition fixed;
    fixed.mCoarse = MAX(0, dp.mCoarse);
    fixed.mFine = MAX(0, dp.mFine);
    fixed.mSuperFine = MAX(0, dp.mSuperFine);

    if (fixed.mCoarse < mCoarseRadix)
    {
      if (fixed.mFine < mFineRadix)
      {
        if (fixed.mSuperFine < mSuperFineRadix)
        {
          // do nothing
        }
        else
        {
          fixed.mSuperFine = mSuperFineRadix - 1;
        }
      }
      else
      {
        fixed.mFine = mFineRadix - 1;
        fixed.mSuperFine = 0;
      }
    }
    else
    {
      fixed.mCoarse = mCoarseRadix;
      fixed.mFine = 0;
      fixed.mSuperFine = 0;
    }
    return fixed;
  }

  void DialMap::getRadixWeights(float &coarseWeight, float &fineWeight,
                                float &superFineWeight, float amplitude)
  {
    coarseWeight = amplitude / mCoarseRadix;
    fineWeight = coarseWeight / mFineRadix;
    superFineWeight = fineWeight / mSuperFineRadix;
  }

} /* namespace od */
