#pragma once

#include <od/audio/OnsetDetector.h>
#include <od/ui/EncoderHysteresis.h>
#include <od/graphics/sampling/SliceHeadDisplay.h>

namespace od
{

  class SlicingViewMainDisplay : public SliceHeadDisplay
  {
  public:
    SlicingViewMainDisplay(SliceHead *head);
    virtual ~SlicingViewMainDisplay();

    void insertSlice(bool snapToZeroCrossing = false);
    void deleteSlice();
    void clearSlices();

    bool haveSliceUnderPointer();

    void setLoopStart(bool snapToZeroCrossing = false);
    void setLoopEnd(bool snapToZeroCrossing = false);

    void insertOnsetSlices(bool snapToZeroCrossing = false);
    void insertPeriodSlices(bool snapToZeroCrossing = false);
    void insertDivisionSlices(bool snapToZeroCrossing = false);
    void shiftSlices(bool wrap = true);

    void movePointerByProportion(float amount);
    void movePointerInSamples(int amount);
    void movePointerToHead();
    int movePointerToNearestZeroCrossing();
    void movePointerToViewCenter();
    void setPointer(int position);
    int getPointer()
    {
      return mPointer;
    }

    void beginMarking();
    void endMarking();
    void clearMarking();
    void updateMarking();
    bool isMarked();
    float getMarkedDuration();
    float getMarkedBeginTime();
    float getMarkedEndTime();
    int getMarkStart()
    {
      return mSampleView.mMarkStart;
    }
    int getMarkEnd()
    {
      return mSampleView.mMarkEnd;
    }

    void enableFollowing()
    {
      mFollowSliceHead = true;
    }

    void disableFollowing()
    {
      mFollowSliceHead = false;
    }

    bool isFollowing()
    {
      return mFollowSliceHead;
    }

    bool encoderPointer(int change, bool shifted, float sensitivity);
    bool encoderOnsetThreshold(int change, bool shifted, float sensitivity);
    bool encoderGridPeriod(int change, bool shifted, int sensitivity);
    bool encoderGridDivision(int change, bool shifted, int sensitivity);
    bool encoderSliceShift(int change, bool shifted, int sensitivity);

    void showOnsetGadget();
    void showPeriodGadget();
    void showDivisionGadget();
    void showShiftGadget();
    void hideGadget();

    float getOnsetThreshold()
    {
      return mOnsetThreshold;
    }

    void forceRefresh()
    {
      mSampleView.forceRefresh();
    }

#ifndef SWIGLUA
    virtual void draw(FrameBuffer &fb);
    void drawGridHelper(FrameBuffer &fb);
    void drawShiftHelper(FrameBuffer &fb);
    void drawOnsetHelper(FrameBuffer &fb);
    void drawStatus(FrameBuffer &fb);
    int findNearestZeroCrossing(Sample *pSample, int pos, int windowStart,
                                int windowEnd);

    // pointer
    EncoderHysteresis mEncoderHysteresis;
    int mPointer = 0;
    int mAnchor = 0;
    bool mFollowSliceHead = false;
    int mPendingAnchor = 0;
    int mMarkState = 0; // 0 - not marking, 1 - waiting for movement, 2 - marking

    enum Gadget
    {
      none,
      onsetGadget,
      periodGadget,
      divisionGadget,
      shiftGadget,
    };

    Gadget mActiveGadget = Gadget::none;

    // onset
    OnsetDetector slicer;
    Slices mGadgetSlices;
    float mOnsetThreshold = 5.0f;
    Label mOnsetGadget{"", 10};

    Label mGridGadget{"", 10};
    // grid period
    int mGridPeriodInMS = 500; // in milliseconds
    double mGridPeriod = 0.5;  // in seconds
    // grid division
    int mGridDivision = 4;

    // shift
    int mSliceShiftInMS = 0;  // in milliseconds
    float mSliceShift = 0.0f; // in seconds
    Label mShiftGadget{"", 10};
#endif

  protected:
    void updateGridPeriodFromGridDivision();
  };

} /* namespace od */
