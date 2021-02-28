#pragma once

#include <od/graphics/Graphic.h>
#include <od/graphics/text/Label.h>
#include <od/audio/Sample.h>

namespace od
{

  class SampleView : public Graphic
  {
  public:
    SampleView(int left, int bottom, int width, int height);
    virtual ~SampleView();

    bool setSample(Sample *sample);
    void setChannel(int channel);

    bool encoderZoom(int change, bool shifted, int threshold);

    void setMarkVisible(bool value)
    {
      mShowMarkedRegion = value;
    }

    void setMarkColor(Color color)
    {
      mMarkColor = color;
    }

    void forceRefresh();
    virtual void setSize(int width, int height);

    void showTimeZoomGadget()
    {
      mZoomGadgetState = showTimeGadget;
    }

    void showGainZoomGadget()
    {
      mZoomGadgetState = showGainGadget;
    }

    void hideZoomGadget()
    {
      mZoomGadgetState = gadgetHidden;
    }

    float getViewTimeInSeconds();
    void setViewTimeInSeconds(float secs);

    int getViewGainInDecibels()
    {
      return mGainDecibels;
    }
    void setViewGainInDecibels(int decibels);

#ifndef SWIGLUA
    virtual void draw(FrameBuffer &fb);
    virtual void notifyVisible();
    void drawPosition(FrameBuffer &fb, int position, const char *name,
                      int width = 5);
    void drawMiniPosition(FrameBuffer &fb, int position, int height, int y);
    void drawPositionOverview(FrameBuffer &fb, int position);
    void drawTimeZoomGadget(FrameBuffer &fb);
    void drawGainZoomGadget(FrameBuffer &fb);
    void refresh(int start);
    void invalidateInterval(int from, int to);

    void setCenterPosition(int position);
    void setLeftPosition(int position);
    void setRightPosition(int position);

    void setMarkedRegion(int start, int end)
    {
      mMarkStart = start;
      mMarkEnd = end;
    }

    bool encoderZoomTime(int change, bool shifted, int threshold);
    bool encoderZoomGain(int change, bool shifted, int threshold);

    // view
    int mStart = 0;
    int mEnd = 0;
    int mZoomLevel = 1;

    // mark
    int mMarkStart = 0;
    int mMarkEnd = 0;
    bool mShowMarkedRegion = false;
    Color mMarkColor = WHITE;

    // width in pixels = mPixelsPerSample * sample count
    float mPixelsPerSample;
    // sample count = mSamplesPerPixel * width in pixels
    int mSamplesPerPixel;
#endif

  protected:
    Sample *mpSample = 0;
    uint32_t mSavedWaterMark = 0;
    int mStartSaved = 0;
    int mZoomLevelSaved = 0;
    int mLastLoadCount = 0;
    int mGainDecibels = 0;
    int mChannel = 0;
    float mGain = 1.0f;
    std::vector<int> mMaximums;
    std::vector<int> mMinimums;
    struct Interval
    {
      Interval(int from, int to) : from(from), to(to)
      {
      }
      int from;
      int to;
    };
    std::vector<Interval> mInvalidatedIntervals;

    enum ZoomGadgetState
    {
      gadgetHidden,
      showTimeGadget,
      showGainGadget
    };
    ZoomGadgetState mZoomGadgetState = gadgetHidden;

    Label mTimeZoomGadget{"", 10};
    float mViewSizeInSeconds = 0.0f;
    float mSavedViewSizeInSeconds = -1.0f;
    Label mGainZoomGadget{"", 10};
    int mSavedViewGain = -9999;
    int mEncoderSum = 0;

    void setZoomLevel(int level);
    void prepareVectors();
    void partialRefresh(int start, int sampleCount);
  };

} /* namespace od */
