#include <od/objects/heads/SliceHead.h>
#include <hal/simd.h>
#include <hal/ops.h>
#include <math.h>

namespace od
{

  SliceHead::SliceHead()
  {
    addInput(mSliceSelect);
    addInput(mSliceShift);
    addOption(mAddressOption);
    addOption(mHowOften);
    addOption(mHowMuch);
    addOption(mPolarity);
    addOption(mSustain);
    addParameter(mFadeTime);
    addParameter(mActualPlayDuration);
  }

  SliceHead::~SliceHead()
  {
    if (mpSlices)
      mpSlices->release();
  }

  void SliceHead::setSample(Sample *sample)
  {
    setSample(sample, NULL);
  }

  void SliceHead::setSample(Sample *sample, Slices *slices)
  {
    Base::setSample(sample);

    if (mpSample)
    {
      if (fabs(mpSample->mSampleRate - globalConfig.sampleRate) < 0.5f)
      {
        mSpeedAdjustment = 1.0f;
        mSpeedAdjustmentEnabled = false;
      }
      else
      {
        mSpeedAdjustment = sample->mSampleRate * globalConfig.samplePeriod;
        mSpeedAdjustmentEnabled = true;
      }

      mPreviousBehavior = determineBehavior(false);
    }
    else
    {
      mSpeedAdjustment = 1.0f;
      mSpeedAdjustmentEnabled = false;
    }

    Slices *pSlices = mpSlices;
    mpSlices = 0;
    if (pSlices)
    {
      pSlices->release();
    }

    pSlices = slices;
    mActiveSliceIndex = 0;
    if (pSlices)
    {
      pSlices->attach();
      mpSlices = pSlices;
      refreshSliceSelection();
    }
    mpSlices = pSlices;
  }

  static inline int wrap(int shift, int n, int N)
  {
    n += shift;
    if (n < N)
    {
      return n;
    }
    else
    {
      return n - N;
    }
  }

  void SliceHead::refreshSliceSelection()
  {
    if (mManualMode)
    {
      return;
    }

    if (mpSlices->size() == 0)
    {
      if (mActiveSliceIndex >= 0)
      {
        // There was a slice active before, let's deactivate it now.
        mActiveSliceIndex = -1;
        setReset(0);
      }
      return;
    }

    float selection = *(mSliceSelect.buffer() + globalConfig.frameLength - 1);
    SlicesIterator i = mpSlices->end();

    if (selection < 0)
    {
      selection = 0;
    }
    else if (selection > 1.0f)
    {
      selection = 1.0f;
    }

    switch (mAddressOption.value())
    {
    case SLICEADDRESS_SAMPLE:
    {
      int start = selection * (mpSample->mSampleCount - 1);
      i = mpSlices->findNearest(start);
      break;
    }
    case SLICEADDRESS_INDEX:
    {
      int j = selection * ((int)mpSlices->size() - 1);
      i = mpSlices->begin() + j;
      break;
    }
    case SLICEADDRESS_CHROMATIC:
    {
      int j = selection * FULLSCALE_IN_VOLTS * 12 + 0.5f;
      if (j >= mpSlices->size())
      {
        j = mpSlices->size() - 1;
      }
      i = mpSlices->begin() + j;
      break;
    }
    }

    if (i != mpSlices->end())
    {
      float *shift = mSliceShift.buffer() + globalConfig.frameLength - 1;
      int n = (int)((*shift) * mpSample->mSampleRate);
      setActiveSlice(i, n);
    }
  }

  void SliceHead::setActiveSlice(int index, int shift)
  {
    if (mpSlices && mpSlices->isIndexValid(index))
    {
      SlicesIterator i = mpSlices->begin() + index;
      setActiveSlice(i, shift);
    }
  }

  void SliceHead::setActiveSlice(SlicesIterator &i, int shift)
  {
    Slice &slice = *i;
    mActiveSliceIndex = i - mpSlices->begin();
    mActiveSliceShift = shift;

    SlicesIterator j = i + 1;
    if (shift == 0)
    {
      setReset(slice.mStart);
      if (j == mpSlices->end())
      {
        // This is the last slice...
        if (i == mpSlices->begin())
        {
          // Only one slice...
          setStops(0, mEndIndex);
          setLoops(0, slice.mStart, slice.mLoopStart, slice.mLoopEnd);
        }
        else
        {
          // Last slice of more than one slice.
          SlicesIterator h = i - 1;
          Slice &prevSlice = *h;
          setStops(prevSlice.mStart, mEndIndex);
          setLoops(prevSlice.mLoopStart, prevSlice.mLoopEnd,
                   slice.mLoopStart, slice.mLoopEnd);
        }
      }
      else
      {
        Slice &nextSlice = *j;
        if (i == mpSlices->begin())
        {
          // First slice with a slice afterwards...
          setStops(0, nextSlice.mStart);
          setLoops(0, slice.mStart, slice.mLoopStart, slice.mLoopEnd);
        }
        else
        {
          SlicesIterator h = i - 1;
          Slice &prevSlice = *h;
          setStops(prevSlice.mStart, nextSlice.mStart);
          setLoops(prevSlice.mLoopStart, prevSlice.mLoopEnd,
                   slice.mLoopStart, slice.mLoopEnd);
        }
      }
    }
    else
    {
      int N = mEndIndex;
      int M = N - 1;

      // clamp and wrap shift to [0,M]
      if (shift > 0)
      {
        shift = MIN(shift, M);
      }
      else
      {
        shift = MAX(shift, -M) + N;
      }

      // set reset
      setReset(wrap(shift, slice.mStart, N));

      // set stop points
      if (j == mpSlices->end())
      {
        if (i == mpSlices->begin())
        {
          setStops(shift, wrap(shift, M, N));
        }
        else
        {
          SlicesIterator h = i - 1;
          Slice &prevSlice = *h;
          setStops(wrap(shift, prevSlice.mStart, N), wrap(shift, N, N));
        }
      }
      else
      {
        Slice &nextSlice = *j;
        if (i == mpSlices->begin())
        {
          setStops(shift, wrap(shift, nextSlice.mStart, N));
        }
        else
        {
          SlicesIterator h = i - 1;
          Slice &prevSlice = *h;
          setStops(wrap(shift, prevSlice.mStart, N),
                   wrap(shift, nextSlice.mStart, N));
        }
      }
    }
  }

  SliceHead::Behavior SliceHead::determineBehavior(bool sustain)
  {
    int howOften = mHowOften.value();
    bool playSlice, playCue;

    if (mManualMode)
    {
      playSlice = true;
    }
    else
    {
      playSlice = mHowMuch.value() == HOWMUCH_SLICE && haveActiveSlice();
      playCue = mHowMuch.value() == HOWMUCH_CUE && haveActiveSlice();
    }

    if (howOften == HOWOFTEN_GATE)
    {
      if (playSlice)
      {
        // One-shot Slices
        switch (mPolarity.value())
        {
        case SLICEPOLARITY_LEFT:
          if (sustain)
          {
            return Behavior(mLeftLoopStartIndex, mLeftLoopEndIndex,
                            mLeftLoopEndIndex, mLeftLoopStartIndex,
                            mLeftStopIndex, mLeftResetIndex,
                            mLeftResetIndex - mLeftStopIndex,
                            mLeftResetIndex - mLeftStopIndex);
          }
          else
          {
            return Behavior(mLeftResetIndex, mLeftResetIndex,
                            mLeftStopIndex, mLeftStopIndex,
                            mLeftStopIndex, mLeftResetIndex,
                            mLeftResetIndex - mLeftStopIndex,
                            mLeftResetIndex - mLeftStopIndex);
          }
        case SLICEPOLARITY_BOTH:
          if (sustain)
          {
            return Behavior(mRightLoopEndIndex, mRightLoopStartIndex,
                            mLeftLoopStartIndex, mLeftLoopEndIndex,
                            mLeftResetIndex, mLeftResetIndex,
                            mRightStopIndex - mLeftResetIndex,
                            mLeftResetIndex - mLeftStopIndex);
          }
          else
          {
            return Behavior(mRightStopIndex, mRightStopIndex,
                            mLeftStopIndex, mLeftStopIndex,
                            mLeftResetIndex, mLeftResetIndex,
                            mRightStopIndex - mLeftResetIndex,
                            mLeftResetIndex - mLeftStopIndex);
          }
        case SLICEPOLARITY_RIGHT:
        default:
          if (sustain)
          {
            return Behavior(mRightLoopEndIndex, mRightLoopStartIndex,
                            mRightLoopStartIndex, mRightLoopEndIndex,
                            mLeftResetIndex, mRightStopIndex,
                            mRightStopIndex - mLeftResetIndex,
                            mRightStopIndex - mLeftResetIndex);
          }
          else
          {
            return Behavior(mRightStopIndex, mRightStopIndex,
                            mLeftResetIndex, mLeftResetIndex,
                            mLeftResetIndex, mRightStopIndex,
                            mRightStopIndex - mLeftResetIndex,
                            mRightStopIndex - mLeftResetIndex);
          }
        }
      }
      else if (playCue)
      {
        if (sustain)
        {
          // Play from cue then loop all.
          return Behavior(mEndIndex, 0, 0, mEndIndex, mLeftResetIndex,
                          mLeftResetIndex, mEndIndex, mEndIndex);
        }
        else
        {
          // Play from cue then stop.
          return Behavior(mEndIndex, mEndIndex, 0, 0, mLeftResetIndex,
                          mLeftResetIndex, mEndIndex, mEndIndex);
        }
      }
      else
      {
        if (sustain)
        {
          // Looping All
          return Behavior(mEndIndex, 0, 0, mEndIndex, 0, mEndIndex,
                          mEndIndex, mEndIndex);
        }
        else
        {
          // One-shot All
          return Behavior(mEndIndex, mEndIndex, 0, 0, 0, mEndIndex,
                          mEndIndex, mEndIndex);
        }
      }
    }
    else if (howOften == HOWOFTEN_LOOP)
    {
      if (playSlice)
      {
        int polarity = mPolarity.value();
        if (mLeftResetIndex == 0)
        {
          polarity = SLICEPOLARITY_RIGHT;
        }
        else if (mLeftResetIndex == mEndIndex)
        {
          polarity = SLICEPOLARITY_LEFT;
        }
        // Looping Slices
        switch (polarity)
        {
        case SLICEPOLARITY_LEFT:
          return Behavior(mLeftResetIndex, mLeftStopIndex, mLeftStopIndex,
                          mLeftResetIndex, mLeftStopIndex,
                          mLeftResetIndex,
                          mLeftResetIndex - mLeftStopIndex,
                          mLeftResetIndex - mLeftStopIndex);
        case SLICEPOLARITY_BOTH:
          return Behavior(mRightStopIndex, mLeftResetIndex,
                          mLeftStopIndex, mLeftResetIndex,
                          mLeftResetIndex, mLeftResetIndex,
                          mRightStopIndex - mLeftResetIndex,
                          mLeftResetIndex - mLeftStopIndex);
        case SLICEPOLARITY_RIGHT:
        default:
          return Behavior(mRightStopIndex, mLeftResetIndex,
                          mLeftResetIndex, mRightStopIndex,
                          mLeftResetIndex, mRightStopIndex,
                          mRightStopIndex - mLeftResetIndex,
                          mRightStopIndex - mLeftResetIndex);
        }
      }
      else if (playCue)
      {
        // Play from cue then loop all.
        return Behavior(mEndIndex, 0, 0, mEndIndex, mLeftResetIndex,
                        mLeftResetIndex, mEndIndex, mEndIndex);
      }
      else
      {
        // Looping All
        return Behavior(mEndIndex, 0, 0, mEndIndex, 0, mEndIndex, mEndIndex,
                        mEndIndex);
      }
    }
    else
    {
      if (playSlice)
      {
        // One-shot Slices
        switch (mPolarity.value())
        {
        case SLICEPOLARITY_LEFT:
          return Behavior(mLeftResetIndex, mLeftResetIndex,
                          mLeftStopIndex, mLeftStopIndex, mLeftStopIndex,
                          mLeftResetIndex,
                          mLeftResetIndex - mLeftStopIndex,
                          mLeftResetIndex - mLeftStopIndex);
        case SLICEPOLARITY_BOTH:
          return Behavior(mRightStopIndex, mRightStopIndex,
                          mLeftStopIndex, mLeftStopIndex, mLeftResetIndex,
                          mLeftResetIndex,
                          mRightStopIndex - mLeftResetIndex,
                          mLeftResetIndex - mLeftStopIndex);
        case SLICEPOLARITY_RIGHT:
        default:
          return Behavior(mRightStopIndex, mRightStopIndex,
                          mLeftResetIndex, mLeftResetIndex,
                          mLeftResetIndex, mRightStopIndex,
                          mRightStopIndex - mLeftResetIndex,
                          mRightStopIndex - mLeftResetIndex);
        }
      }
      else if (playCue)
      {
        // Play to end/begin then stop.
        return Behavior(mEndIndex, mEndIndex, 0, 0, mLeftResetIndex,
                        mLeftResetIndex, mEndIndex, mEndIndex);
      }
      else
      {
        // One-shot All
        return Behavior(mEndIndex, mEndIndex, 0, 0, 0, mEndIndex, mEndIndex,
                        mEndIndex);
      }
    }
  }

  int SliceHead::getFirstTrigger(float *trigger)
  {
    if (mManualMode)
    {
      return FRAMELENGTH; // no trigger
    }
    else
    {
      return simd_first_positive_4x(trigger, FRAMELENGTH);
    }
  }

} // namespace od
