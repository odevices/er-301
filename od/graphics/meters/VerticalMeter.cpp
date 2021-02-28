#include <od/graphics/meters/VerticalMeter.h>
#include <od/AudioThread.h>
#include <od/extras/LookupTables.h>
#include <hal/constants.h>
#include <hal/log.h>
#include <stdio.h>
#include <math.h>

namespace od
{

  VerticalMeter::VerticalMeter(int left, int bottom, int width, int height) : Graphic(left, bottom, width, height)
  {
    own(mProbe);
  }

  VerticalMeter::~VerticalMeter()
  {
    disconnectProbe();
    clearOutlet();
  }

  void VerticalMeter::calculate()
  {
    if (mpWatchedOutlet == 0)
    {
      mDecibels = -160;
      return;
    }

    float rms = mProbe.value();
    if (rms > 0)
    {
      mDecibels = MAX(-160, 20 * log10f(rms));
    }
    else
    {
      mDecibels = -160;
    }
  }

  const std::string &VerticalMeter::getDecibelText()
  {
    int disp = (int)mDecibels;
    if (mDisplayDecibels != disp)
    {
      if (mDisplayTextUpdateCount == 0)
      {
        char tmp[16];
        int height;
        mDisplayDecibels = disp;
        snprintf(tmp, sizeof(tmp), "%02ddB", mDisplayDecibels);
        getTextSize(tmp, &mDisplayTextWidth, &height, 10);
        mDisplayText = tmp;
        mDisplayTextUpdateCount = GRAPHICS_REFRESH_RATE / 4;
      }
      mDisplayTextUpdateCount--;
    }
    return mDisplayText;
  }

  void VerticalMeter::draw(FrameBuffer &fb)
  {
    Graphic::draw(fb);

    if (mCalculateCount == RefreshTime)
    {
      calculate();
      mCalculateCount = 0;
    }
    else
    {
      mCalculateCount++;
    }

    int h0 = (int)((mHeight - 1) * (60.0f) / 72.0f);
    fb.box(GRAY7, mWorldLeft, mWorldBottom, mWorldLeft + mWidth,
           mWorldBottom + mHeight);
    fb.hline(GRAY11, mWorldLeft, mWorldLeft + mWidth, mWorldBottom + h0);

    if (mCalculateCount < 0)
    {
      return;
    }

    int h = (int)((mHeight - 1) * (60.0f + mDecibels) / 72.0f);
    if (h < 0)
      h = 0;
    if (h > mHeight - 1)
      h = mHeight - 1;
    fb.fill(mForeground, mWorldLeft, mWorldBottom, mWorldLeft + mWidth,
            mWorldBottom + h);

    if (mDisplayTextEnabled)
    {
      if (mDisplayTextLeft)
      {
        fb.text(WHITE, mWorldLeft - mDisplayTextWidth - 2, mWorldBottom + 5,
                getDecibelText().c_str(), 10);
      }
      else
      {
        fb.text(WHITE, mWorldLeft + mWidth + 2, mWorldBottom + 5,
                getDecibelText().c_str(), 10);
      }
    }
  }

  void VerticalMeter::watchOutlet(Outlet *outlet)
  {
    disconnectProbe();
    clearOutlet();
    mpWatchedOutlet = outlet;
    if (mpWatchedOutlet)
    {
      mpWatchedOutlet->attach();
      if (mVisibility == visibleState)
      {
        connectProbe();
      }
    }
  }

  void VerticalMeter::connectProbe()
  {
    if (mProbe.scheduled())
      return;

    if (mpWatchedOutlet)
    {
      AudioThread::connect(mpWatchedOutlet, &mProbe.mInput, &mProbe);
    }

    mCalculateCount = -WarmUpTime;
    mDecibels = -160;
  }

  void VerticalMeter::disconnectProbe()
  {
    if (mProbe.scheduled())
    {
      AudioThread::disconnect(&mProbe.mInput, &mProbe);
    }
  }

  void VerticalMeter::clearOutlet()
  {
    if (mpWatchedOutlet)
    {
      mpWatchedOutlet->release();
      mpWatchedOutlet = 0;
    }
  }

  void VerticalMeter::notifyHidden()
  {
    logDebug(10, "VerticalMeter(%s): hidden", getWatchName().c_str());
    disconnectProbe();
    Graphic::notifyHidden();
  }

  void VerticalMeter::notifyVisible()
  {
    logDebug(10, "VerticalMeter(%s): visible", getWatchName().c_str());
    connectProbe();
    Graphic::notifyVisible();
  }

  std::string VerticalMeter::getWatchName()
  {
    if (mpWatchedOutlet)
    {
      return mpWatchedOutlet->mName;
    }
    return "None";
  }

} /* namespace od */
