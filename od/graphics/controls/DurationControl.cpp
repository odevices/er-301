/*
 * DurationControl.cpp
 *
 *  Created on: 8 Sep 2016
 *      Author: clarkson
 */

#include <od/graphics/controls/DurationControl.h>
#include <stdio.h>

namespace od
{

    DurationControl::DurationControl() : Graphic(0, 0, 100, 20)
    {
        mCursorState.orientation = cursorDown;
    }

    DurationControl::~DurationControl()
    {
    }

    static void MMSSXXX(float total, std::string &result)
    {
        char tmp[32];
        int min = (int)total / 60.0f;
        int secs = (int)(total - min * 60.0f);
        int ms = (int)(1000.0f * (total - (int)total) + 0.5f);
        snprintf(tmp, sizeof(tmp), "%02dm %02ds %03dms", min, secs, ms);
        result = tmp;
    }

    static void SSXXX(float total, std::string &result)
    {
        char tmp[32];
        int secs = (int)total;
        int ms = (int)(1000.0f * (total - secs) + 0.5f);
        snprintf(tmp, sizeof(tmp), "%4ds %03dms", secs, ms);
        result = tmp;
    }

    static void beats(float total, int bpm, std::string &result)
    {
        char tmp[32];
        int beats = (int)(total * bpm / 60.0f + 0.5f);
        snprintf(tmp, sizeof(tmp), "%4d @ %3d BPM", beats, bpm);
        result = tmp;
    }

    void DurationControl::draw(FrameBuffer &fb)
    {

        //if (mDisplayedValue != mSeconds) {
        // value has changed, update display text
        //	mDisplayedValue = mSeconds;

        switch (mMode)
        {
        case durationMMSSXXX:
            MMSSXXX(mSeconds, mDisplayText);
            break;
        case durationSSXXX:
            SSXXX(mSeconds, mDisplayText);
            break;
        case durationBeats:
            beats(mSeconds, mBaseTempo, mDisplayText);
            break;
        }

        getTextSize(mDisplayText.c_str(), &mTextWidth, &mTextHeight, mTextSize);
        Point center = getCenter();
        mHeight = mTextHeight;
        mWidth = mTextWidth;
        setCenter(center.x, center.y);

        mCursorState.y = mWorldBottom + mHeight + 10;
        switch (mMode)
        {
        case durationMMSSXXX:
            switch (mEncoderFocus)
            {
            case 0:
                mCursorState.x = mWorldLeft + mWidth * (1.5f / 13);
                break;
            case 1:
                mCursorState.x = mWorldLeft + mWidth * (5.5f / 13);
                break;
            case 2:
                mCursorState.x = mWorldLeft + mWidth * (9.5f / 13);
                break;
            }
            break;
        case durationSSXXX:
            switch (mEncoderFocus)
            {
            case 0:
                mCursorState.x = mWorldLeft + mWidth * (1.5f / 13);
                break;
            case 1:
                mCursorState.x = mWorldLeft + mWidth * (5.5f / 13);
                break;
            case 2:
                mCursorState.x = mWorldLeft + mWidth * (9.5f / 13);
                break;
            }
            break;
        case durationBeats:
            switch (mEncoderFocus)
            {
            case 0:
                mCursorState.x = mWorldLeft + mWidth * (2.0f / 15);
                break;
            case 1:
                mCursorState.x = mWorldLeft + mWidth * (2.0f / 15);
                break;
            case 2:
                mCursorState.x = mWorldLeft + mWidth * (8.5 / 15);
                break;
            }
            break;
        }
        //	}

        Graphic::draw(fb);

        fb.text(mForeground, mWorldLeft, mWorldBottom, mDisplayText.c_str(),
                mTextSize);
    }

    void DurationControl::increment()
    {
        switch (mMode)
        {
        case durationMMSSXXX:
            switch (mEncoderFocus)
            {
            case 0:
                setTotalSeconds(mSeconds + 60.0f);
                break;
            case 1:
                setTotalSeconds(mSeconds + 1.0f);
                break;
            case 2:
                setTotalSeconds(mSeconds + 0.001f);
                break;
            }
            break;
        case durationSSXXX:
            switch (mEncoderFocus)
            {
            case 0:
                setTotalSeconds(mSeconds + 1.0f);
                break;
            case 1:
                setTotalSeconds(mSeconds + 1.0f);
                break;
            case 2:
                setTotalSeconds(mSeconds + 0.001f);
                break;
            }
            break;
        case durationBeats:
            switch (mEncoderFocus)
            {
            case 0:
            case 1:
                setTotalSeconds(60.0f * (getBeats() + 1) / mBaseTempo);
                break;
            case 2:
                setBaseTempo(mBaseTempo + 1);
                break;
            }
            break;
        }
    }

    void DurationControl::decrement()
    {
        switch (mMode)
        {
        case durationMMSSXXX:
            switch (mEncoderFocus)
            {
            case 0:
                setTotalSeconds(mSeconds - 60.0f);
                break;
            case 1:
                setTotalSeconds(mSeconds - 1.0f);
                break;
            case 2:
                setTotalSeconds(mSeconds - 0.001f);
                break;
            }
            break;
        case durationSSXXX:
            switch (mEncoderFocus)
            {
            case 0:
                setTotalSeconds(mSeconds - 1.0f);
                break;
            case 1:
                setTotalSeconds(mSeconds - 1.0f);
                break;
            case 2:
                setTotalSeconds(mSeconds - 0.001f);
                break;
            }
            break;
        case durationBeats:
            switch (mEncoderFocus)
            {
            case 0:
            case 1:
                setTotalSeconds(60.0f * (getBeats() - 1) / mBaseTempo);
                break;
            case 2:
                setBaseTempo(mBaseTempo - 1);
                break;
            }
            break;
        }
    }

    bool DurationControl::encoder(int change, bool shifted, int threshold)
    {
        bool triggered = false;
        mEncoderSum += change;

        while (mEncoderSum > threshold)
        {
            increment();
            mEncoderSum -= threshold;
            triggered = true;
        }

        while (mEncoderSum < -threshold)
        {
            decrement();
            mEncoderSum += threshold;
            triggered = true;
        }

        return triggered;
    }

    void DurationControl::setEncoderFocus(int focus)
    {
        mDisplayedValue = -1.0f;
        mEncoderFocus = focus;
    }

} /* namespace od */
