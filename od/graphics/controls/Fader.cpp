#include <od/graphics/controls/Fader.h>
#include <od/AudioThread.h>
#include <hal/log.h>
#include <math.h>

namespace od
{

  Fader::Fader(int left, int bottom, int width, int height) : Graphic(left, bottom, width, height), mReadout(0, 15, width, 10)
  {
    own(mReadout);
    own(mNameLabel);
    own(mLeftProbe);
    own(mRightProbe);

    addChild(&mReadout);
    addChild(&mNameLabel);
    mNameLabel.setCenter(mWidth / 2);

    mCursorState.orientation = cursorRight;
  }

  Fader::~Fader()
  {
    disconnectProbes();
    clearOutlets();
    setValueParameter(0);
    setTargetParameter(0);
    setControlParameter(0);
    setRangeObject(0);
  }

  void Fader::calculateLeftMeter()
  {
    if (mpLeftOutlet == 0)
    {
      mLeftDecibels = -160;
      return;
    }

    float rms = mLeftProbe.value();
    if (rms > 0)
    {
      mLeftDecibels = MAX(-160, 20 * log10f(rms));
    }
    else
    {
      mLeftDecibels = -160;
    }
  }

  void Fader::calculateRightMeter()
  {
    if (mpRightOutlet == 0)
    {
      mRightDecibels = -160;
      return;
    }

    float rms = mRightProbe.value();
    if (rms > 0)
    {
      mRightDecibels = MAX(-160, 20 * log10f(rms));
    }
    else
    {
      mRightDecibels = -160;
    }
  }

  void Fader::draw(FrameBuffer &fb)
  {
    Graphic::draw(fb);

    int x = mWorldLeft + mWidth / 2, y;
    int y0 = mReadout.mWorldBottom + mReadout.mHeight;
    int y1 = mWorldBottom + mHeight - 4;
    int H = y1 - y0;

    if (mpRangeObject)
    {
      int x0 = x + 6;

      // draw Min/Max slot
      fb.vline(GRAY7, x0, y0, y1);
      fb.hline(GRAY7, x0 - 2, x0 + 2, y0);
      fb.hline(GRAY7, x0 - 2, x0 + 2, y1);

      int yMin, yMax;
      float max = mReadout.convertToUnits(mpMaximum->value());
      float min = mReadout.convertToUnits(mpMinimum->value());
      yMax = y0 + (int)(H * scale(max));
      yMin = y0 + (int)(H * scale(min));
      if (yMax > y1)
      {
        yMax = y1;
      }
      if (yMin < y0)
      {
        yMin = y0;
      }
      fb.fill(mForeground, x0 - 1, yMin, x0 + 1, yMax);

      // shift main fader to the left
      x -= 6;
    }

    if (mpLeftOutlet)
    {
      calculateLeftMeter();

      int y0db = y0 + (int)(H * (60.0f) / 72.0f);
      y = y0 + (int)(H * (60.0f + mLeftDecibels) / 72.0f);
      y = MIN(MAX(y, y0), y1);

      int x0 = mWorldLeft + 3 * mWidth / 4 - 5;
      if (mpRightOutlet)
      {
        x0 -= 1;
      }
      fb.box(GRAY7, x0 - 2, y0, x0 + 2, y1);
      fb.hline(GRAY11, x0 - 2, x0 + 2, y0db);
      fb.fill(mForeground, x0 - 2, y0, x0 + 2, y);

      x = mWorldLeft + mWidth / 4 + 5;
    }

    if (mpRightOutlet)
    {
      calculateRightMeter();

      int y0db = y0 + (int)(H * (60.0f) / 72.0f);
      y = y0 + (int)(H * (60.0f + mRightDecibels) / 72.0f);
      y = MIN(MAX(y, y0), y1);

      int x0 = mWorldLeft + 3 * mWidth / 4;
      fb.box(GRAY7, x0 - 2, y0, x0 + 2, y1);
      fb.hline(GRAY11, x0 - 2, x0 + 2, y0db);
      fb.fill(mForeground, x0 - 2, y0, x0 + 2, y);

      x = mWorldLeft + mWidth / 4;
    }

    // draw fader slot
    mFaderX = x;
    fb.vline(GRAY7, x, y0, y1);
    fb.hline(GRAY7, x - 2, x + 2, y0);
    fb.hline(GRAY7, x - 2, x + 2, y1);

    // draw target position
    if (mpTargetParameter)
    {
      float target = mReadout.convertToUnits(mpTargetParameter->target());
      y = y0 + (int)(H * scale(target));
      y = MIN(MAX(y, y0), y1);
      fb.clear(x - 3, y - 1, x + 3, y + 1);
      if (mHighlightTarget)
      {
        fb.box(mForeground, x - 3, y - 1, x + 3, y + 1);
      }
      else
      {
        fb.box(GRAY7, x - 3, y - 1, x + 3, y + 1);
      }
      mCursorState.x = mWorldLeft;
      mCursorState.y = y;
    }

    // draw value position
    if (mpValueParameter)
    {
      float value = mReadout.convertToUnits(mpValueParameter->value());
      y = y0 + (int)(H * scale(value));
      y = MIN(MAX(y, y0), y1);
      // fb.box(GRAY7, x - 3, y - 1, x + 3, y + 1);
      if (mHighlightTarget)
      {
        fb.hline(GRAY7, x - 4, x + 4, y);
      }
      else
      {
        fb.hline(mForeground, x - 4, x + 4, y);
      }
    }

    // draw zero position
    y = y0 + (int)(H * mScaledZero);
    y = MIN(MAX(y, y0), y1);
    fb.hline(GRAY11, x - 2, x + 2, y);
  }

  void Fader::clearOutlets()
  {
    if (mpLeftOutlet)
    {
      mpLeftOutlet->release();
      mpLeftOutlet = 0;
    }
    if (mpRightOutlet)
    {
      mpRightOutlet->release();
      mpRightOutlet = 0;
    }
  }

  void Fader::watchOutlets(Outlet *left, Outlet *right)
  {
    if (mpLeftOutlet == left && mpRightOutlet == right)
      return;

    disconnectProbes();
    clearOutlets();

    mpLeftOutlet = left;
    if (mpLeftOutlet)
    {
      mpLeftOutlet->attach();
    }
    mpRightOutlet = right;
    if (mpRightOutlet)
    {
      mpRightOutlet->attach();
    }

    if (mVisibility == visibleState)
    {
      connectProbes();
    }
  }

  void Fader::connectProbes()
  {
    if (mpLeftOutlet && !mLeftProbe.scheduled())
    {
      AudioThread::connect(mpLeftOutlet, &mLeftProbe.mInput, &mLeftProbe);
    }
    if (mpRightOutlet && !mRightProbe.scheduled())
    {
      AudioThread::connect(mpRightOutlet, &mRightProbe.mInput, &mRightProbe);
    }
  }

  void Fader::disconnectProbes()
  {
    if (mLeftProbe.scheduled())
    {
      AudioThread::disconnect(&mLeftProbe.mInput, &mLeftProbe);
    }
    if (mRightProbe.scheduled())
    {
      AudioThread::disconnect(&mRightProbe.mInput, &mRightProbe);
    }
  }

  std::string Fader::getLeftWatchName()
  {
    if (mpLeftOutlet)
    {
      return mpLeftOutlet->mName;
    }
    return "None";
  }

  std::string Fader::getRightWatchName()
  {
    if (mpRightOutlet)
    {
      return mpRightOutlet->mName;
    }
    return "None";
  }

  void Fader::notifyHidden()
  {
    logDebug(10, "Fader(%s,%s): hidden", getLeftWatchName().c_str(), getRightWatchName().c_str());
    disconnectProbes();
    Graphic::notifyHidden();
  }

  void Fader::notifyVisible()
  {
    logDebug(10, "Fader(%s,%s): visible", getLeftWatchName().c_str(), getRightWatchName().c_str());
    connectProbes();
    Graphic::notifyVisible();
  }

  static inline float log2(float x)
  {
    // return logf(x) * (1.0f / logf(2.0f));
    return logf(x) * (1.44269504f);
  }

  float Fader::scale(float x)
  {
    switch (mScaling)
    {
    case linearScaling:
    case decibelScaling:
      break;
    case octaveScaling:
      x = log2(x);
      break;
    }
    return (x - mScaledMinimum) * mScale;
  }

  void Fader::setScaling(Scaling scaling)
  {
    mScaling = scaling;
    refreshScaling();
  }

  void Fader::setMap(DialMap *map)
  {
    mReadout.setMap(map);
    refreshScaling();
  }

  void Fader::setUnits(ReadoutUnits units)
  {
    mReadout.setUnits(units);
    refreshScaling();
  }

  void Fader::setAttributes(ReadoutUnits units, DialMap *map, Scaling scaling)
  {
    mReadout.setMap(map);
    mReadout.setUnits(units);
    mScaling = scaling;
    refreshScaling();
  }

  void Fader::setPrecision(int p)
  {
    mReadout.setPrecision(p);
  }

  void Fader::refreshScaling()
  {
    DialMap *map = mReadout.mDialState.getMap();
    if (map)
    {
      switch (mScaling)
      {
      case linearScaling:
      case decibelScaling:
        mScaledMinimum = map->min();
        mScale = 1.0f / (map->max() - mScaledMinimum);
        mScaledZero = mReadout.convertToUnits(0.0f);
        break;
      case octaveScaling:
        mScaledMinimum = log2(map->min(true));
        mScale = 1.0f / (log2(map->max()) - mScaledMinimum);
        mScaledZero = log2(mReadout.convertToUnits(0.0f) + 1e-10f);
        break;
      }
    }
  }

  void Fader::setTextBelow(float value, const std::string &text)
  {
    mReadout.setTextBelow(value, text);
  }

  void Fader::setTextAbove(float value, const std::string &text)
  {
    mReadout.setTextAbove(value, text);
  }

  void Fader::save()
  {
    mReadout.save();
  }

  void Fader::restore()
  {
    mReadout.restore();
  }

  void Fader::zero()
  {
    mReadout.zero();
  }

  void Fader::encoder(int change, bool shifted, bool fine)
  {
    mReadout.encoder(change, shifted, fine);
  }

  void Fader::grayed()
  {
    mForeground = GRAY5;
  }

  void Fader::ungrayed()
  {
    mForeground = WHITE;
  }

  Parameter *Fader::getValueParameter()
  {
    return mpValueParameter;
  }

  void Fader::setValueParameter(Parameter *param)
  {
    if (mpValueParameter)
    {
      mpValueParameter->release();
    }
    mpValueParameter = param;
    if (mpValueParameter)
    {
      mpValueParameter->attach();
    }
  }

  Parameter *Fader::getTargetParameter()
  {
    return mpTargetParameter;
  }

  void Fader::setTargetParameter(Parameter *param)
  {
    if (mpTargetParameter)
    {
      mpTargetParameter->release();
    }
    mpTargetParameter = param;
    if (mpTargetParameter)
    {
      mpTargetParameter->attach();
    }
  }

  Parameter *Fader::getControlParameter()
  {
    return mpControlParameter;
  }

  void Fader::setControlParameter(Parameter *param)
  {
    if (mpControlParameter)
    {
      mpControlParameter->release();
    }
    mpControlParameter = param;
    if (mpControlParameter)
    {
      mpControlParameter->attach();
    }
    mReadout.setParameter(mpControlParameter);
  }

  void Fader::setParameter(Parameter *param)
  {
    setValueParameter(param);
    setTargetParameter(param);
    setControlParameter(param);
  }

  Object *Fader::getRangeObject()
  {
    return mpRangeObject;
  }

  void Fader::setRangeObject(Object *object)
  {
    if (mpRangeObject)
    {
      mpMinimum = 0;
      mpMaximum = 0;
      mpRangeObject->release();
    }
    mpRangeObject = object;
    if (mpRangeObject)
    {
      mpRangeObject->attach();
      mpMinimum = mpRangeObject->getParameter("Min");
      mpMaximum = mpRangeObject->getParameter("Max");
      if (mpMinimum == 0 || mpMaximum == 0)
      {
        mpRangeObject->release();
        mpRangeObject = 0;
        mpMinimum = 0;
        mpMaximum = 0;
      }
    }
  }

  void Fader::setLabel(const std::string &text)
  {
    mNameLabel.setText(text);
    mNameLabel.setCenter(mWidth / 2);
  }

  const std::string &Fader::getLabel()
  {
    return mNameLabel.getText();
  }

  void Fader::highlightTarget()
  {
    mHighlightTarget = true;
  }

  void Fader::highlightValue()
  {
    mHighlightTarget = false;
  }

} /* namespace od */
