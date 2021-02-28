#pragma once

#include <od/graphics/Graphic.h>
#include <od/objects/Parameter.h>
#include <od/ui/DialState.h>
#include <limits>

namespace od
{

  enum ReadoutUnits
  {
    unitNone,
    // gain
    unitDecibels, // dB
    // freq
    unitHertz,      // Hz
    unitCents,      // cents symbol
    unitMultiplier, // 1x, 2x, etc.
    // time
    unitSecs,  // s, ms, us
    unitBeats, // 1/32, 1/24, 1/16, 1/12, 1/8, 1/6, 1/4, 1/3, 1/2, 1, 2, ...
    unitSamples,
    //
    unitPercent
  };

  class Fader;

  class Readout : public Graphic
  {
  public:
    Readout(int left, int bottom, int width, int textSize);
    virtual ~Readout();

#ifndef SWIGLUA
    virtual void draw(FrameBuffer &fb);
    float convertToUnits(float x);
    float convertFromUnits(float x);
#endif

    void setJustification(Justification mode);
    void setAttributes(ReadoutUnits units, DialMap *map);
    void setMap(DialMap *map);
    void setUnits(ReadoutUnits units);
    ReadoutUnits getUnits();
    void setPrecision(int p);
    int getPrecision();
    void save();
    void restore();
    void zero();
    void encoder(int change, bool shifted, bool fine);
    void setTextBelow(float value, const std::string &text);
    void setTextAbove(float value, const std::string &text);
    void useSoftSet();
    void useHardSet();

    float getValueInUnits();
    void setValueInUnits(float x);

    // Parameter encapsulation
    float value();
    float target();
    void hardSet(float x);
    void softSet(float x);

    static std::string formatQuantity(float value, ReadoutUnits units,
                                      int precision,
                                      bool suppressTrailingZeros);

    Parameter *getParameter();
    void setParameter(Parameter *param);

  protected:
    Parameter *mpParameter = 0;
    DialState mDialState;
    ReadoutUnits mUnits = unitNone;
    int mPrecision = 3;
    float mLastSetTarget = std::numeric_limits<float>::max();
    float mLastValue = std::numeric_limits<float>::max();
    float mDisplayValue = std::numeric_limits<float>::max();
    bool mShowTarget = true;
    int mSuppressTrailingZeros = 0;

    int mTextWidth, mTextHeight;
    std::string mText;
    int mTextSize;

    Justification mJustification = justifyCenter;
    bool mCursorVisible = false;

    float mMinTextThreshold = std::numeric_limits<float>::lowest();
    std::string mMinText;
    float mMaxTextThreshold = std::numeric_limits<float>::max();
    std::string mMaxText;

    bool mUseHardSet = false;

    void commitChanges(bool force);

    friend Fader;
  };

} /* namespace od */
