/*
 * SixDigits.cpp
 *
 *  Created on: 1 Oct 2018
 *      Author: clarkson
 */

#include <od/graphics/controls/SixDigits.h>
#include <math.h>

#define VOFFSET 0

namespace od
{

    static int steps[] = {100000, 10000, 1000, 100, 10, 1};
    static float powersOfTen[] = {0.00001, 0.0001, 0.001, 0.01, 0.1, 1};

    SixDigits::SixDigits() : Graphic(0, 0, 256, 64)
    {
        refresh();
    }

    SixDigits::~SixDigits()
    {
    }

    void SixDigits::draw(FrameBuffer &fb)
    {
        Graphic::draw(fb);
        for (int i = 0; i < 6; i++)
        {
            drawDigit(fb, i + 1, mDigits[i], mColors[i], justifyCenter);
        }

        if (mMantissa < 0)
        {
            int y = mWorldBottom + mHeight / 2 + VOFFSET;
            fb.text(WHITE, mWorldLeft, y, "-", 48, ALIGN_MIDDLE);
        }

        fb.hline(WHITE, mWorldLeft, mWorldLeft + mWidth,
                 mWorldBottom + mHeight - 12);

        fb.text(WHITE, mWorldLeft, mWorldBottom + mHeight - 6, mMessage.c_str(), 10,
                ALIGN_MIDDLE);
    }

    void SixDigits::refresh()
    {
        int tmp;

        if (mMantissa < 0)
        {
            tmp = -mMantissa;
        }
        else
        {
            tmp = mMantissa;
        }

        mDigits[5] = tmp % 10;
        tmp /= 10;
        mDigits[4] = tmp % 10;
        tmp /= 10;
        mDigits[3] = tmp % 10;
        tmp /= 10;
        mDigits[2] = tmp % 10;
        tmp /= 10;
        mDigits[1] = tmp % 10;
        tmp /= 10;
        mDigits[0] = tmp % 10;

        bool found = false;
        for (int i = 0; i < 6; i++)
        {
            if (found)
            {
                mColors[i] = WHITE;
            }
            else if (mDigits[i] != 0 || mDecimalPoint == i + 1)
            {
                found = true;
                mColors[i] = GRAY13;
            }
            else
            {
                mColors[i] = GRAY2;
            }
        }
    }

    static int buttonCenters[] = {
        BUTTON1_CENTER,
        BUTTON2_CENTER,
        BUTTON3_CENTER,
        BUTTON4_CENTER,
        BUTTON5_CENTER,
        BUTTON6_CENTER};

    void SixDigits::drawDigit(FrameBuffer &fb, int i, int value, Color color,
                              Justification justification)
    {
        bool dp = mDecimalPoint == i;
        bool selected = mSelectedDigit == i;
        char tmp[4] = {0, 0, 0, 0};
        int y = mWorldBottom + mHeight / 2 + VOFFSET;

        tmp[0] = '0' + value;
        if (dp)
        {
            tmp[1] = '.';
        }

        int textLeft = mWorldLeft + (i - 1) * 40;

        switch (justification)
        {
        case justifyLeft:
            textLeft += mMargin;
            break;
        case justifyCenter:
            textLeft += SECTION_PLY / 2;
            break;
        case justifyRight:
            textLeft += SECTION_PLY - mMargin;
            break;
        }

        Color digitColor;
        if (selected)
        {
            digitColor = CLAMP(BLACK, WHITE, GRAY5 + sTween * GRAY10);
        }
        else
        {
            digitColor = color;
        }

        fb.text(digitColor, textLeft, y, tmp, 48, ALIGN_MIDDLE);

        int nextTextLeft = mWorldLeft + i * 40;
        int center = (textLeft + nextTextLeft) / 2 + 2;
        int bottom = y - 15;

        Color decorColor = GRAY3;
        if (selected)
        {
            decorColor = GRAY7;
            if (mDecimalPointSelected)
            {
                int textWidth, textHeight;
                getTextSize(tmp, &textWidth, &textHeight, 48);
                mCursorState.orientation = cursorUp;
                mCursorState.x = textLeft + textWidth - 3;
                mCursorState.y = bottom - 10;
                mCursorState.show = true;
            }
            else
            {
                mCursorState.show = false;
                fb.fill(decorColor, textLeft, bottom - 6, nextTextLeft, bottom - 4);
            }
        }

        if (!mDecimalPointSelected)
        {
            fb.line(decorColor, buttonCenters[i - 1], mWorldBottom, center,
                    bottom - 4);
        }
    }

    bool SixDigits::increment()
    {
        int step = steps[mSelectedDigit - 1];
        if (mMantissa < 0 && mMantissa + step > 0)
        {
            // going through zero
            mMantissa = step - mMantissa;
            refresh();
            return true;
        }
        else
        {
            if (mMantissa + step < mMantissaCeiling + 1)
            {
                mMantissa += step;
                refresh();
                return true;
            }
            else if (mDecimalPoint < 6 && mSelectedDigit < 6)
            {
                mMantissa /= 10;
                mDecimalPoint++;
                mSelectedDigit++;
                return increment();
            }
            else
            {
                return false;
            }
        }
    }

    bool SixDigits::decrement()
    {
        int step = steps[mSelectedDigit - 1];
        if (mMantissa > 0 and mMantissa - step < 0)
        {
            // going through zero
            mMantissa = -step - mMantissa;
            refresh();
            return true;
        }
        else
        {
            if (mMantissa - step >= mMantissaFloor)
            {
                mMantissa -= step;
                refresh();
                return true;
            }
            else if (mDecimalPoint < 6 && mSelectedDigit < 6)
            {
                mMantissa /= 10;
                mDecimalPoint++;
                mSelectedDigit++;
                return decrement();
            }
            else
            {
                return false;
            }
        }
    }

    bool SixDigits::encoder(int change, bool shifted, int threshold)
    {
        bool changed = false;
        mEncoderSum += change;

        while (mEncoderSum > threshold)
        {
            mEncoderSum -= threshold;
            if (mDecimalPointSelected)
            {
                if (mDecimalPoint < 6)
                {
                    mDecimalPoint++;
                    mSelectedDigit = mDecimalPoint;
                    refresh();
                    changed = true;
                }
            }
            else
            {
                changed = changed || increment();
            }
        }

        while (mEncoderSum < -threshold)
        {
            mEncoderSum += threshold;
            if (mDecimalPointSelected)
            {
                if (mDecimalPoint > 1)
                {
                    mDecimalPoint--;
                    mSelectedDigit = mDecimalPoint;
                    refresh();
                    changed = true;
                }
            }
            else
            {
                changed = changed || decrement();
            }
        }

        return changed;
    }

    void SixDigits::selectDigit(int i)
    {
        mDecimalPointSelected = false;
        mSelectedDigit = CLAMP(1, 6, i);
    }
    void SixDigits::selectDigit()
    {
        mDecimalPointSelected = false;
    }

    void SixDigits::selectDecimalPoint()
    {
        mDecimalPointSelected = true;
        mSelectedDigit = mDecimalPoint;
    }
    void SixDigits::setDecimalPoint(int i)
    {
        mDecimalPoint = CLAMP(1, 6, i);
        refresh();
    }

    float SixDigits::getValue()
    {
        return mMantissa * powersOfTen[mDecimalPoint - 1];
    }

    int SixDigits::getIntegerPart()
    {
        int result = 0;
        for (int i = 0; i < mDecimalPoint; i++)
        {
            result += mDigits[i] * steps[i + 6 - mDecimalPoint];
        }

        if (mMantissa < 0)
        {
            return -result;
        }

        return result;
    }

    int SixDigits::getDecimalPart()
    {
        int result = 0;
        for (int i = mDecimalPoint; i < 6; i++)
        {
            result += mDigits[i] * steps[i];
        }
        return result;
    }

    bool SixDigits::isInteger()
    {
        for (int i = mDecimalPoint; i < 6; i++)
        {
            if (mDigits[i] != 0)
            {
                return false;
            }
        }

        return true;
    }

    void SixDigits::setValue(float value)
    {
        value = CLAMP(mMantissaFloor, mMantissaCeiling, value);
        mDecimalPoint = 6;
        float absValue = fabs(value);
        if (absValue < 1.0)
        {
            mDecimalPoint = 1;
            absValue *= 100000;
        }
        else
        {
            for (int i = 0; i < 6; i++)
            {
                if (absValue < 100000)
                {
                    absValue *= 10;
                    mDecimalPoint--;
                }
            }
        }

        mMantissa = (int)(absValue + 0.5f);
        if (value < 0)
        {
            mMantissa *= -1;
        }
        refresh();
    }

    void SixDigits::setIntegerValue(int value)
    {
        mMantissa = CLAMP(mMantissaFloor, mMantissaCeiling, value);
        mDecimalPoint = 6;
        refresh();
    }

    void SixDigits::setMantissaFloor(int value)
    {
        mMantissaFloor = value;
        mMantissa = MAX(mMantissaFloor, mMantissa);
        refresh();
    }

    void SixDigits::setMantissaCeiling(int value)
    {
        mMantissaCeiling = value;
        mMantissa = MIN(mMantissaCeiling, mMantissa);
        refresh();
    }

    int SixDigits::getDecimalPoint()
    {
        return mDecimalPoint;
    }

    void SixDigits::zero()
    {
        mMantissa = 0;
        refresh();
    }

    void SixDigits::zeroRight()
    {
        for (int i = mSelectedDigit; i < 6; i++)
        {
            if (mMantissa > 0)
            {
                mMantissa -= mDigits[i] * steps[i];
            }
            else
            {
                mMantissa += mDigits[i] * steps[i];
            }
        }
        refresh();
    }

    void SixDigits::zeroLeft()
    {
        for (int i = 0; i < mSelectedDigit - 1; i++)
        {
            if (mMantissa > 0)
            {
                mMantissa -= mDigits[i] * steps[i];
            }
            else
            {
                mMantissa += mDigits[i] * steps[i];
            }
        }
        refresh();
    }

    void SixDigits::setMessage(const std::string &msg)
    {
        mMessage = msg;
    }

} /* namespace od */
