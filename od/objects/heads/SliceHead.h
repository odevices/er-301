#pragma once

#include <od/objects/heads/TapeHead.h>
#include <od/audio/Slices.h>

#define SLICEPOLARITY_LEFT 1
#define SLICEPOLARITY_BOTH 2
#define SLICEPOLARITY_RIGHT 3

#define SLICEADDRESS_SAMPLE 1
#define SLICEADDRESS_INDEX 2
#define SLICEADDRESS_CHROMATIC 3

namespace od
{

  class SampleDisplay;
  class SampleSubDisplay;

  class SliceHead : public TapeHead
  {
  public:
    SliceHead();
    virtual ~SliceHead();

    virtual void setSample(Sample *sample);
    virtual void setSample(Sample *sample, Slices *slices);

    int getActiveSliceIndex()
    {
      return mActiveSliceIndex;
    }

    void setActiveSlice(int index, int shift = 0);

    int getActiveSliceShift()
    {
      return mActiveSliceShift;
    }

    bool haveActiveSlice()
    {
      return mActiveSliceIndex >= 0;
    }

    void setManualMode(bool value)
    {
      mManualMode = value;
    }

#ifndef SWIGLUA
    Inlet mSliceSelect{"Slice Select"};
    Inlet mSliceShift{"Slice Shift"};
    Option mAddressOption{"Address", SLICEADDRESS_INDEX};
    Option mHowOften{"How Often", HOWOFTEN_LOOP};
    Option mHowMuch{"How Much", HOWMUCH_ALL};
    Option mPolarity{"Polarity", SLICEPOLARITY_BOTH};
    Option mSustain{"Sustain", 0};
    Parameter mFadeTime{"Fade Time", 0.002f};
    Parameter mActualPlayDuration{"Actual Play Duration"};

    Slices *mpSlices = NULL;
    int mActiveSliceIndex = -1;
    int mActiveSliceShift = 0;
    bool mManualMode = false;

    // Used to adjust to the sample rate of the sample.
    float mSpeedAdjustment = 1.0f;
    bool mSpeedAdjustmentEnabled = false;

    void refreshSliceSelection();
    void setActiveSlice(SlicesIterator &i, int shift);
    Slice *getActiveSlice()
    {
      if (mpSlices)
      {
        return mpSlices->get(mActiveSliceIndex);
      }
      else
      {
        return 0;
      }
    }

#endif

  protected:
    struct Behavior
    {
      Behavior()
      {
      }
      Behavior(int forwardGoal, int forwardJump, int reverseGoal,
               int reverseJump, int forwardReset, int reverseReset,
               int forwardDuration, int reverseDuration) : forwardGoal(forwardGoal), forwardJump(forwardJump), reverseGoal(reverseGoal), reverseJump(reverseJump), forwardReset(forwardReset), reverseReset(reverseReset), forwardDuration(forwardDuration), reverseDuration(reverseDuration)
      {
      }

      int forwardGoal;
      int forwardJump;
      int reverseGoal;
      int reverseJump;
      int forwardReset;
      int reverseReset;
      int forwardDuration;
      int reverseDuration;
    };
    Behavior determineBehavior(bool sustain = false);
    Behavior mPreviousBehavior;

    int getFirstTrigger(float *trigger);

  private:
    typedef TapeHead Base;
  };

} /* namespace od */
