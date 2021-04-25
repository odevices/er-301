#include <od/ui/DialState.h>

namespace od
{

  DialState::DialState()
  {
  }

  DialState::~DialState()
  {
    if (mpMap)
    {
      mpMap->release();
      mpMap = 0;
    }
  }

  void DialState::setMap(DialMap *map)
  {
    if (mpMap)
    {
      mpMap->release();
    }
    mpMap = map;
    if (mpMap)
    {
      mpMap->attach();
      setWithValue(mValue);
    }
  }

  float DialState::set(const DialPosition &dp)
  {
    if (mpMap)
    {
      mPosition = mpMap->validatePosition(dp);
      mValue = mpMap->getValue(mPosition);
    }
    return mValue;
  }

  void DialState::setWithValue(float value)

  {
    if (mpMap)
    {
      mPosition = mpMap->getPosition(value);
      mValue = mpMap->getValue(mPosition);
    }
  }

  float DialState::encoder(int change, bool shifted, bool fine)
  {
    if (mpMap == 0)
      return mValue;

    change = mEncoderHysteresis.onChange(change);
    if (change == 0)
    {
      return mValue;
    }

    if (shifted)
    {
      if (fine)
      {
        // Super Fine (10x)
        mpMap->updateSuperFine(mPosition, change);
      }
      else
      {
        // Super Coarse (x10)
        DialPosition saved = mPosition;
        int M = mpMap->superCoarseMultiplier();
        mPosition.mFine = 0;
        mPosition.mSuperFine = 0;
        mPosition.mCoarse = M * (mPosition.mCoarse / M);
        if (change > 0 || mPosition == saved)
        {
          mpMap->updateCoarse(mPosition, change * M);
        }
      }
    }
    else
    {
      if (fine)
      {
        // Fine
        DialPosition saved = mPosition;
        mPosition.mSuperFine = 0;
        if (change > 0 || mPosition == saved)
        {
          mpMap->updateFine(mPosition, change);
        }
      }
      else
      {
        // Coarse
        DialPosition saved = mPosition;
        mPosition.mFine = 0;
        mPosition.mSuperFine = 0;
        if (change > 0 || mPosition == saved)
        {
          mpMap->updateCoarse(mPosition, change);
        }
      }
    }

    mValue = mpMap->getValue(mPosition);
    return mValue;
  }

  float DialState::getValue()
  {
    return mValue;
  }

  float DialState::zero()
  {
    if (mpMap)
    {
      return set(mpMap->getZeroPosition());
    }
    else
    {
      return mValue;
    }
  }

  void DialState::save()
  {
    mSavedPosition = mPosition;
    mSaved = true;
  }

  float DialState::restore()
  {
    if (mSaved)
    {
      DialPosition savedPosition = mSavedPosition;
      save();
      return set(savedPosition);
    }
    else
    {
      return zero();
    }
  }

} /* namespace od */
