#pragma once

#include <od/ui/DialMap.h>

namespace od
{

  class LinearDialMap : public DialMap
  {
  public:
    LinearDialMap(float min = 0, float max = 1);
    LinearDialMap(const LinearDialMap &map) = default;
    virtual ~LinearDialMap();

#ifndef SWIGLUA
    virtual float getValue(const DialPosition &dp);
    virtual DialPosition getPosition(float value);
#endif

    virtual float min(bool skipZero = false);
    virtual float max();

    void setMin(float min)
    {
      mMinimum = min;
      markChanged();
    }

    void setMax(float max)
    {
      mMaximum = max;
      markChanged();
    }

    void setSteps(float superCoarse, float coarse, float fine,
                  float superFine);

    float superCoarseStep();
    float coarseStep();
    float fineStep();
    float superFineStep();

  private:
    float mMinimum;
    float mMaximum;
    float mCoarseWeight;
    float mFineWeight;
    float mSuperFineWeight;

    void commitChanges();
  };

} /* namespace od */
