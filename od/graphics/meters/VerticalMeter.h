#pragma once

#include <od/objects/measurement/LoudnessProbe.h>
#include <od/graphics/Graphic.h>

namespace od
{

  class VerticalMeter : public Graphic
  {
  public:
    VerticalMeter(int left, int bottom, int width, int height);
    virtual ~VerticalMeter();

#ifndef SWIGLUA
    virtual void draw(FrameBuffer &fb);
#endif
    void watchOutlet(Outlet *outlet);
    void clearOutlet();

    void enableDisplayText()
    {
      mDisplayTextEnabled = true;
    }

    void disableDisplayText()
    {
      mDisplayTextEnabled = false;
    }

    void displayTextOnLeft()
    {
      mDisplayTextLeft = true;
    }

    void displayTextOnRight()
    {
      mDisplayTextLeft = false;
    }

    const std::string &getDecibelText();

  private:
    LoudnessProbe mProbe;
    Outlet *mpWatchedOutlet = 0;
    float mDecibels = -160;
    float mPeak = 0;

    int mDisplayDecibels = 0;
    std::string mDisplayText;
    bool mDisplayTextEnabled = false;
    bool mDisplayTextLeft = false;
    int mDisplayTextUpdateCount = 0;
    int mDisplayTextWidth = 0;

    void calculate();
    int mCalculateCount = 0;
    static const int RefreshTime = 2;
    static const int WarmUpTime = 10;

    void connectProbe();
    void disconnectProbe();

    virtual void notifyHidden();
    virtual void notifyVisible();
    std::string getWatchName();
  };

} /* namespace od */
