#pragma once

#include <od/ui/DialMap.h>
#include <od/ui/LinearDialMap.h>
#include <vector>

namespace od
{

  class LUTDialMap : public DialMap
  {
  public:
    LUTDialMap(int size, int coarseMultiple = 1, int fineRadix = 10, int superFineRadix = 10);
    virtual ~LUTDialMap();

#ifndef SWIGLUA
    virtual float getValue(const DialPosition &dp);
    virtual DialPosition getPosition(float value);
#endif

    virtual float min(bool skipZero = false);
    virtual float max();

    void add(float value);
    void disableInterpolation()
    {
      mInterpolated = false;
    }

  private:
    std::vector<float> mValues;
    int mCount;
    float mCoarseWeight;
    float mFineWeight;
    float mSuperFineWeight;
    bool mInterpolated = true;

    void commitChanges();
  };

} /* namespace od */
