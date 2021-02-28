/*
 * Comparator.h
 *
 *  Created on: Jun 11, 2016
 *      Author: clarkson
 */

#ifndef APP_OBJECTS_TIMING_COMPARATOR_H_
#define APP_OBJECTS_TIMING_COMPARATOR_H_

#include <od/objects/Object.h>
#include <od/config.h>

#define COMPARATOR_TOGGLE 1
#define COMPARATOR_GATE 2
#define COMPARATOR_TRIGGER_ON_RISE 3
#define COMPARATOR_TRIGGER_ON_FALL 4

namespace od
{

  class Comparator : public Object
  {
  public:
    Comparator();
    virtual ~Comparator();

#ifndef SWIGLUA
    virtual void process();
    Inlet mInput{"In"};
    Outlet mOutput{"Out"};
    Parameter mThreshold{"Threshold", 0.1f};
    Parameter mHysteresis{"Hysteresis", 0.03f};
    Option mState{"State", 0};
    Parameter mLastEdgeCount{"Last Edge Count", 0};
    Parameter mLastLevelCount{"Last Level Count", 0};
    Option mMode{"Mode", COMPARATOR_TRIGGER_ON_RISE};
#endif

    void simulateRisingEdge();
    void simulateFallingEdge();

    void resetCounter()
    {
      mResetCounter = true;
    }

    int getLevelCount()
    {
      return mLevelCount;
    }

    int getRisingEdgeCount()
    {
      return mRisingEdgeCount;
    }

    float getElapsed()
    {
      return mSecondsSinceCounterReset;
    }

    float getRateInHertz()
    {
      if (mSecondsSinceCounterReset > 0.0f)
      {
        return mRisingEdgeCount / mSecondsSinceCounterReset;
      }
      else
      {
        return 0;
      }
    }

    float getRateInBPM()
    {
      return getRateInHertz() * 60.0f;
    }

    float getPeriodInSeconds()
    {
      if (mRisingEdgeCount)
      {
        return mSecondsSinceCounterReset / mRisingEdgeCount;
      }
      else
      {
        return 0;
      }
    }

    void setTriggerMode()
    {
      mMode.set(COMPARATOR_TRIGGER_ON_RISE);
    }

    void setTriggerOnRiseMode()
    {
      mMode.set(COMPARATOR_TRIGGER_ON_RISE);
    }

    void setTriggerOnFallMode()
    {
      mMode.set(COMPARATOR_TRIGGER_ON_FALL);
    }

    void setToggleMode()
    {
      mMode.set(COMPARATOR_TOGGLE);
    }

    void setGateMode()
    {
      mMode.set(COMPARATOR_GATE);
    }

    void setMode(int mode)
    {
      mMode.set(mode);
    }

    int getMode()
    {
      return mMode.value();
    }

    void setOutputInversion(bool value)
    {
      mInvertOutput = value;
    }

    bool isOutputInverted()
    {
      return mInvertOutput;
    }

    void hold() override;
    void unhold() override;

    bool isHeld()
    {
      return mHeld;
    }

    bool isRisingEdgeHeld()
    {
      return mRisingEdgeHeld;
    }

    bool isFallingEdgeHeld()
    {
      return mFallingEdgeHeld;
    }

  private:
    bool mResetCounter = false;
    bool mSimulateRisingEdge = false;
    bool mSimulateFallingEdge = false;
    bool mSimulatedLevel = false;
    bool mInvertOutput = false;
    bool mHeld = false;
    bool mRisingEdgeHeld = false;
    bool mFallingEdgeHeld = false;

    // measurements
    int mLevelCount = 0;
    int mRisingEdgeCount = 0;
    float mSecondsSinceCounterReset = 0;
    float mSecondsSinceLast = 0;

    void processTriggerOnRise();
    void processTriggerOnFall();
    void processToggle();
    void processGate();
  };

} /* namespace od */

#endif /* APP_OBJECTS_TIMING_COMPARATOR_H_ */
