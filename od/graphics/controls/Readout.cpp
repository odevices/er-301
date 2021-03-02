#include <od/graphics/controls/Readout.h>
#include <od/extras/Conversions.h>
#include <od/extras/Utils.h>
#include <utility>
#include <stdio.h>

namespace od
{

  static void quantityToString(std::string &result, float value,
                               ReadoutUnits units, int precision,
                               bool suppressTrailingZeros)
  {
    float magnitude = fabs(value);
    const char *suffix;

    switch (units)
    {
    case unitDecibels:
      suffix = "dB";
      precision = MIN(2, precision);
      break;
    case unitHertz:
      suffix = "Hz";
      break;
    case unitCents:
      suffix = "\xa2"; // ¢
      break;
    case unitMultiplier:
      suffix = "x";
      break;
    case unitSecs:
      if (magnitude == 0)
      {
        suffix = "s";
      }
      else if (magnitude < 0.1f - EPSILON)
      {
        suffix = "ms";
        value *= 1000;
        magnitude *= 1000;
        precision = MAX(0, precision - 3);
      }
      else
      {
        suffix = "s";
      }
      break;
    case unitPercent:
      suffix = "%";
      break;
    case unitNone:
    default:
      suffix = "";
      break;
    }

    if (magnitude > 10000 - EPSILON)
    {
      precision = 0;
    }
    else if (magnitude > 1000 - EPSILON)
    {
      precision = MIN(1, precision);
    }
    else if (magnitude > 100 - EPSILON)
    {
      precision = MIN(2, precision);
    }

    char tmp[16];

    switch (precision)
    {
    case 0:
      snprintf(tmp, sizeof(tmp), "%d%s", fastRound(value), suffix);
      result = tmp;
      return;
    case 1:
      snprintf(tmp, sizeof(tmp), "%0.1f", value);
      break;
    case 2:
      snprintf(tmp, sizeof(tmp), "%0.2f", value);
      break;
    case 3:
    default:
      snprintf(tmp, sizeof(tmp), "%0.3f", value);
      break;
    }

    result = tmp;
    if (suppressTrailingZeros)
    {
      while (result.back() == '0')
      {
        result.pop_back();
      }
      if (result.back() == '.')
      {
        result.pop_back();
      }
    }

    result += suffix;
  }

  Readout::Readout(int left, int bottom, int width, int textSize) : Graphic(left, bottom, width, textSize), mText("------"), mTextSize(
                                                                                                                                 textSize)
  {
    commitChanges(true);
    mCursorState.orientation = cursorRight;
  }

  Readout::~Readout()
  {
    if (mpParameter)
    {
      mpParameter->release();
    }
  }

  Parameter *Readout::getParameter()
  {
    return mpParameter;
  }

  void Readout::setParameter(Parameter *param)
  {
    if (mpParameter)
    {
      mpParameter->release();
    }
    mpParameter = param;
    if (mpParameter)
    {
      mpParameter->attach();
    }
    mLastValue = std::numeric_limits<float>::max();
  }

  std::string Readout::formatQuantity(float value, ReadoutUnits units,
                                      int precision, bool suppressTrailingZeros)
  {
    std::string result;
    quantityToString(result, value, units, precision, suppressTrailingZeros);
    return result;
  }

  float Readout::convertToUnits(float x)
  {
    switch (mUnits)
    {
    case unitDecibels:
      return toDecibels(x);
    case unitCents:
      return toCents(x);
    case unitPercent:
      return toPercent(x);
    default:
      return x;
    }
  }

  float Readout::convertFromUnits(float x)
  {
    switch (mUnits)
    {
    case unitDecibels:
      return fromDecibels(x);
    case unitCents:
      return fromCents(x);
    case unitPercent:
      return fromPercent(x);
    default:
      return x;
    }
  }

  void Readout::setTextBelow(float value, const std::string &text)
  {
    mMinText = text;
    mMinTextThreshold = value;
  }

  void Readout::setTextAbove(float value, const std::string &text)
  {
    mMaxText = text;
    mMaxTextThreshold = value;
  }

  void Readout::commitChanges(bool force)
  {
    // has the parameter target changed?
    float value = 0.0f;
    if (mpParameter)
    {
      value = mShowTarget ? mpParameter->target() : mpParameter->value();
    }
    bool valueChanged = mLastValue != value;
    if (valueChanged)
    {
      mSuppressTrailingZeros = GRAPHICS_REFRESH_RATE;
    }
    if (force || valueChanged)
    {
      mLastValue = value;
      mDisplayValue = convertToUnits(value);
      if (mDisplayValue < mMinTextThreshold)
      {
        mText = mMinText;
      }
      else if (mDisplayValue > mMaxTextThreshold)
      {
        mText = mMaxText;
      }
      else
      {
        quantityToString(mText, mDisplayValue, mUnits, mPrecision,
                         mSuppressTrailingZeros == 0);
      }
      getTextSize(mText.c_str(), &mTextWidth, &mTextHeight, mTextSize);
    }
  }

  void Readout::draw(FrameBuffer &fb)
  {
    bool force = false;
    Graphic::draw(fb);
    if (mSuppressTrailingZeros > 0)
    {
      mSuppressTrailingZeros--;
      if (mSuppressTrailingZeros == 0)
      {
        force = true;
      }
    }
    commitChanges(force);

    int textLeft;
    switch (mJustification)
    {
    default:
    case justifyLeft:
      textLeft = mWorldLeft;
      break;
    case justifyCenter:
      textLeft = mWorldLeft + (mWidth - mTextWidth) / 2;
      break;
    case justifyRight:
      textLeft = mWorldLeft + mWidth - mTextWidth;
      break;
    }

    fb.text(mForeground, textLeft, mWorldBottom, mText.c_str(), mTextSize);

    mCursorState.x = textLeft - 10;
    mCursorState.y = mWorldBottom + mTextHeight / 2;
  }

  void Readout::encoder(int change, bool shifted, bool fine)
  {
    if (mpParameter)
    {
      float target = mpParameter->target();
      if (fabs(mLastSetTarget - target) > 1e-8)
      {
        mDialState.setWithValue(convertToUnits(target));
      }
      setValueInUnits(mDialState.encoder(change, shifted, fine));
    }
  }

  void Readout::useHardSet()
  {
    mUseHardSet = true;
  }

  void Readout::useSoftSet()
  {
    mUseHardSet = false;
  }

  void Readout::setValueInUnits(float x)
  {
    if (mpParameter)
    {
      mLastSetTarget = convertFromUnits(x);
      if (mUseHardSet)
      {
        mpParameter->hardSet(mLastSetTarget);
      }
      else
      {
        mpParameter->softSet(mLastSetTarget);
      }
    }
  }

  float Readout::getValueInUnits()
  {
    commitChanges(false);
    return mDisplayValue;
  }

  void Readout::save()
  {
    if (mpParameter)
    {
      float target = mpParameter->target();
      if (fabs(mLastSetTarget - target) > 1e-8)
      {
        mDialState.setWithValue(convertToUnits(target));
      }
      mDialState.save();
    }
  }

  void Readout::restore()
  {
    setValueInUnits(mDialState.restore());
  }

  void Readout::zero()
  {
    setValueInUnits(mDialState.zero());
  }

  void Readout::setUnits(ReadoutUnits units)
  {
    if (units != mUnits)
    {
      mUnits = units;
      // force an update
      mLastValue = std::numeric_limits<float>::max();
    }
  }

  void Readout::setMap(DialMap *map)
  {
    if (map)
    {
      mDialState.setMap(map);
      if (getValueInUnits() > map->max())
      {
        setValueInUnits(map->max());
      }
    }
    // force an update
    mLastValue = std::numeric_limits<float>::max();
  }

  void Readout::setAttributes(ReadoutUnits units, DialMap *map)
  {
    setMap(map);
    setUnits(units);
  }

  float Readout::value()
  {
    if (mpParameter)
    {
      return mpParameter->value();
    }
    else
    {
      return 0.0f;
    }
  }

  float Readout::target()
  {
    if (mpParameter)
    {
      return mpParameter->target();
    }
    else
    {
      return 0.0f;
    }
  }

  void Readout::hardSet(float x)
  {
    if (mpParameter)
    {
      mpParameter->hardSet(x);
    }
  }

  void Readout::softSet(float x)
  {
    if (mpParameter)
    {
      mpParameter->softSet(x);
    }
  }

  ReadoutUnits Readout::getUnits()
  {
    return mUnits;
  }

  void Readout::setPrecision(int p)
  {
    int saved = mPrecision;
    mPrecision = MAX(0, p);
    if (saved != mPrecision)
    {
      mSuppressTrailingZeros = GRAPHICS_REFRESH_RATE;
      commitChanges(true);
    }
  }

  int Readout::getPrecision()
  {
    return mPrecision;
  }

  void Readout::setJustification(Justification mode)
  {
    mJustification = mode;
  }

} /* namespace od */
