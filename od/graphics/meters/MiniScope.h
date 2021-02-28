#pragma once

#include <od/objects/measurement/FifoProbe.h>
#include <od/graphics/Graphic.h>
#include <od/graphics/text/Label.h>
#include <od/extras/FastEWMA.h>

namespace od
{

  class MiniScope : public Graphic
  {
  public:
    MiniScope(int left, int bottom, int width, int height);
    virtual ~MiniScope();
#ifndef SWIGLUA
    virtual void draw(FrameBuffer &fb);
#endif
    void watchOutlet(Outlet *outlet);
    void watchInlet(Inlet *inlet);

    void setOffset(float offset)
    {
      mOffset = offset;
    }

    void setLeftBorder(int width)
    {
      mLeftBorder = width;
    }

    void setRightBorder(int width)
    {
      mRightBorder = width;
    }

    void showStatus(bool value)
    {
      mShowStatus = value;
    }

  protected:
    FifoProbe *mpProbe = 0;
    Outlet *mpWatchedOutlet = 0;
    Inlet *mpWatchedInlet = 0;

    float mTriggerThreshold = 0.0f;
    float mOffset = 0.0f;
    int mHorizontalSync = 0;
    std::vector<int> mMaximums;
    std::vector<int> mMinimums;
    FastEWMA mEWMA;

    void calculate();
    int mCalculateCount = 0;
    static const int RefreshTime = 2;
    static const int WarmUpTime = 10;

    int mLeftBorder = 0;
    int mRightBorder = 0;
    bool mShowStatus;

    void connectProbe();
    void disconnectProbe();
    void clearOutlets();

    virtual void notifyHidden();
    virtual void notifyVisible();

    std::string getWatchName();
  };

} /* namespace od */
