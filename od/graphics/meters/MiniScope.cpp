#include <od/graphics/meters/MiniScope.h>
#include <od/AudioThread.h>
#include <od/config.h>
#include <hal/log.h>

#define USE_AUTO_TRIGGER 1
#define MANUAL_TRIGGER_THRESHOLD 0

namespace od
{
  MiniScope::MiniScope(int left, int bottom, int width, int height) : Graphic(left, bottom, width, height)
  {
    mMaximums.resize(mWidth + 1);
    mMinimums.resize(mWidth + 1);
    for (int i = 0; i < mWidth; i++)
    {
      mMaximums[i] = 0;
      mMinimums[i] = 0;
    }
    mEWMA.setInitialState(0.0f);
    mEWMA.setTimeConstant(globalConfig.sampleRate * 0.25, 1.0f);
    mShowStatus = mWidth > 50;
  }

  MiniScope::~MiniScope()
  {
    disconnectProbe();
    clearOutlets();
  }

  void MiniScope::calculate()
  {
    int i, x, y, n;
    float dx, dy;
    float *values;

    if (mpProbe == 0)
    {
      return;
    }

    n = (int)mpProbe->size();
    if (n == 0)
      return;
    values = mpProbe->get(n);

#if USE_AUTO_TRIGGER
    mTriggerThreshold = mEWMA.push(values, n);
#else
    mTriggerThreshold = MANUAL_TRIGGER_THRESHOLD;
#endif

    // search for below-to-above crossing, starting at mid-point
    int n2 = n / 2;
    int n4 = n2 / 2;
    for (i = 1; i < n4; i++)
    {
      if (values[n2 + i - 1] < mTriggerThreshold && values[n2 + i] > mTriggerThreshold)
      {
        mHorizontalSync = i;
        break;
      }
      if (values[n2 - (i - 1)] > mTriggerThreshold && values[n2 - i] < mTriggerThreshold)
      {
        mHorizontalSync = -i;
        break;
      }
    }

    int imin = n2 + mHorizontalSync - n4;
    int imax = imin + n2;

    for (i = 0; i < mWidth; i++)
    {
      mMaximums[i] = -mHeight;
      mMinimums[i] = mHeight;
    }

    dx = (mWidth) / (float)n2;
    dy = (mHeight - 1) / 2.0f;
    for (i = imin; i < imax; i++)
    {
      x = (int)((i - imin) * dx);
      y = (int)((mOffset + values[i]) * dy);
      if (mMaximums[x] < y)
      {
        mMaximums[x] = y;
      }
      if (mMinimums[x] > y)
      {
        mMinimums[x] = y;
      }
    }

    // Remove any gaps
    for (i = 1; i < mWidth; i++)
    {
      mMaximums[i] = MAX(mMaximums[i], mMinimums[i - 1]);
      mMinimums[i] = MIN(mMinimums[i], mMaximums[i - 1]);
    }
  }

  void MiniScope::draw(FrameBuffer &fb)
  {
    Graphic::draw(fb);

    if (mLeftBorder)
    {
      fb.vline(mBorderColor, mWorldLeft, mWorldBottom,
               mWorldBottom + mHeight);
    }

    if (mRightBorder)
    {
      fb.vline(mBorderColor, mWorldLeft + mWidth - 1, mWorldBottom,
               mWorldBottom + mHeight);
    }

    int i, x0, x1, y0, y1;
    int mid = mWorldBottom + mHeight / 2;

    x0 = mWorldLeft;
    x1 = x0 + mWidth - 1;

    y0 = mid;
    fb.hline(GRAY3, x0, x1, y0);
    y0 = mWorldBottom + mHeight / 4;
    fb.hline(GRAY3, x0, x1, y0, 2);
    y0 = mWorldBottom + (3 * mHeight) / 4;
    fb.hline(GRAY3, x0, x1, y0, 2);

    if (mpProbe == 0)
    {
      if (mShowStatus)
      {
        fb.text(WHITE, mWorldLeft + mWidth / 2 - 5, mid + 4, "No", 10);
        fb.text(WHITE, mWorldLeft + mWidth / 2 - 10, mid - 10, "Signal",
                10);
      }
      return;
    }

    if (mCalculateCount >= RefreshTime)
    {
      calculate();
      mCalculateCount = 0;
    }
    else
    {
      mCalculateCount++;
    }

    if (mCalculateCount < 0)
    {
#if 0
        if (mShowStatus)
        {
            fb.text(WHITE, mWorldLeft + mWidth / 2 - 15, mid + 4, "Please", 10);
            fb.text(WHITE, mWorldLeft + mWidth / 2 - 10, mid - 10, "Wait", 10);
        }
        else
        {
            int p = mWidth - mWidth * (-mCalculateCount / (float) WarmUpTime);
            fb.hline(WHITE, mWorldLeft, mWorldLeft + p,
                     mWorldBottom + mHeight - 1);
            fb.hline(WHITE, mWorldLeft, mWorldLeft + p,
                     mWorldBottom + mHeight - 2);
        }
#endif
      return;
    }

    Color color;
    if (fb.mIsMonoChrome)
    {
      color = WHITE;
    }
    else
    {
      color = GRAY3;
    }
    for (i = 0; i < mWidth; i++)
    {
      x0 = mWorldLeft + i;
      y0 = mid + mMinimums[i];
      y1 = mid + mMaximums[i];
      fb.vline(color, x0, y0, y1);
      fb.pixel(mForeground, x0, y0);
      fb.pixel(mForeground, x0, y1);
    }

#ifdef BUILDOPT_VERBOSE
    if (mpProbe)
    {
      if (mpProbe->mInput.mInwardConnection)
      {
        char buffer[128];
        Outlet *outlet = mpProbe->mInput.mInwardConnection;
        Object *object = (Object *)(outlet->owner());
        snprintf(buffer, sizeof(buffer), "%s.%s-->probe",
                 object->name().c_str(), outlet->mName.c_str());
        fb.text(WHITE, mWorldLeft, mWorldBottom + mHeight - 12, buffer, 10);
      }
      else
      {
        fb.text(WHITE, mWorldLeft, mWorldBottom + mHeight - 12,
                "Disconnected probe.", 10);
      }
    }
    else
    {
      fb.text(WHITE, mWorldLeft, mWorldBottom + mHeight - 12,
              "No probe.", 10);
    }
#endif
  }

  void MiniScope::clearOutlets()
  {
    if (mpWatchedOutlet)
    {
      mpWatchedOutlet->release();
      mpWatchedOutlet = 0;
    }

    if (mpWatchedInlet)
    {
      mpWatchedInlet->release();
      mpWatchedInlet = 0;
    }
  }

  void MiniScope::watchOutlet(Outlet *outlet)
  {
    disconnectProbe();
    clearOutlets();
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

  void MiniScope::watchInlet(Inlet *inlet)
  {
    disconnectProbe();
    clearOutlets();
    mpWatchedInlet = inlet;
    if (mpWatchedInlet)
    {
      mpWatchedInlet->attach();
      if (mVisibility == visibleState)
      {
        connectProbe();
      }
    }
  }

  void MiniScope::connectProbe()
  {
    if (mpProbe)
      return;

    if (mpWatchedOutlet)
    {
      mpProbe = AudioThread::getFifoProbe();
      if (mpProbe)
      {
        AudioThread::connect(mpWatchedOutlet, &mpProbe->mInput);
      }
    }
    else if (mpWatchedInlet && mpWatchedInlet->mInwardConnection)
    {
      mpProbe = AudioThread::getFifoProbe();
      if (mpProbe)
      {
        AudioThread::connect(mpWatchedInlet->mInwardConnection,
                             &mpProbe->mInput);
      }
    }

    mCalculateCount = -WarmUpTime;
  }

  void MiniScope::disconnectProbe()
  {
    if (mpProbe)
    {
      AudioThread::disconnect(&mpProbe->mInput);
      AudioThread::releaseFifoProbe(mpProbe);
      mpProbe = 0;
      mEWMA.setInitialState(0.0f);
    }
  }

  std::string MiniScope::getWatchName()
  {
    if (mpWatchedOutlet)
    {
      return mpWatchedOutlet->mName;
    }
    else if (mpWatchedInlet)
    {
      return mpWatchedInlet->mName;
    }
    return "None";
  }

  void MiniScope::notifyHidden()
  {
    logDebug(10, "MiniScope(%s): hidden", getWatchName().c_str());
    disconnectProbe();
    Graphic::notifyHidden();
  }

  void MiniScope::notifyVisible()
  {
    logDebug(10, "MiniScope(%s): visible", getWatchName().c_str());
    connectProbe();
    Graphic::notifyVisible();
  }

} /* namespace od */
