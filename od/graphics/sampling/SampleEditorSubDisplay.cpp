/*
 * SampleEditorSubDisplay.cpp
 *
 *  Created on: 31 Mar 2018
 *      Author: clarkson
 */

#include <od/graphics/sampling/SampleEditorSubDisplay.h>
#include <stdio.h>

namespace od
{

    // micro-adjustments
    const int line1 = GRID6_LINE1 - 3;
    const int line2 = GRID6_LINE2 - 2;
    const int line3 = GRID6_LINE3 - 2;
    const int line4 = GRID6_LINE4 - 2;
    const int line5 = GRID6_LINE5 - 2;

    static void timeString(float totalSecs, std::string &result)
    {
        int hours = totalSecs / 3600;
        totalSecs -= hours * 3600;
        int mins = totalSecs / 60;
        totalSecs -= mins * 60;
        int secs = totalSecs;
        totalSecs -= secs;
        int ms = 1000 * totalSecs;
        char tmp[32];
        snprintf(tmp, sizeof(tmp), "%02d:%02d.%03d", mins, secs, ms);
        result = tmp;
    }

    SampleEditorSubDisplay::SampleEditorSubDisplay(SampleEditorMainDisplay *display) : Graphic(0, 0, 128, 64)
    {
        mpMainDisplay = display;
        if (mpMainDisplay)
        {
            mpMainDisplay->attach();
        }
    }

    SampleEditorSubDisplay::~SampleEditorSubDisplay()
    {
        if (mpMainDisplay)
        {
            mpMainDisplay->release();
        }
        mpMainDisplay = 0;
    }

    void SampleEditorSubDisplay::draw(FrameBuffer &fb)
    {
        uint32_t n;

        if (mpMainDisplay == 0 || mpMainDisplay->head() == 0)
        {
            return;
        }

        Head *pHead = mpMainDisplay->head();
        Sample *pSample = pHead->getSample();

        if (pSample == 0)
        {
            fb.text(WHITE, COL1_LEFT + 5, line3, "No sample.", 10);
            return;
        }

        float sampleRate = pSample->mSampleRate;
        n = pHead->getPosition();
        if (n != mLastHeadPosition)
        {
            mLastHeadPosition = n;
            float totalSecs = ((double)n) / (double)sampleRate;
            timeString(totalSecs, mHeadText);
        }

        n = mpMainDisplay->mPointer;
        if (n != mLastCursorPosition)
        {
            mLastCursorPosition = n;
            float totalSecs = ((double)n) / (double)sampleRate;
            timeString(totalSecs, mCursorText);
        }

        if (pSample->mTotalSeconds != mLastDuration)
        {
            mLastDuration = pSample->mTotalSeconds;
            timeString(mLastDuration, mDurationText);
        }

        // Display name and underline
        fb.text(WHITE, COL1_LEFT, GRID6_LINE1, mName.c_str(), 10);
        fb.hline(WHITE, 0, 128, line1);

        // Display mode
        fb.text(WHITE, COL3_LEFT + 10, line3, "cursor", 10);
        fb.text(WHITE, COL3_LEFT + 10, line4, "follow", 10);

        switch (mMode)
        {
        case cursorMode:
            fb.drawRightTriangle(WHITE, COL3_LEFT + 5, line3 + 3, 3);
            fb.text(WHITE, COL1_LEFT, line2, "cursor:", 10);
            fb.text(WHITE, COL1_LEFT, line3, "head:", 10);
            fb.text(WHITE, COL1_LEFT, line4, "length:", 10);
            fb.text(WHITE, COL2_LEFT, line2, mCursorText.c_str(), 10);
            fb.text(WHITE, COL2_LEFT, line3, mHeadText.c_str(), 10);
            fb.text(WHITE, COL2_LEFT, line4, mDurationText.c_str(), 10);
            break;
        case followMode:
            fb.drawRightTriangle(WHITE, COL3_LEFT + 5, line4 + 3, 3);
            fb.text(WHITE, COL1_LEFT, line2, "cursor:", 10);
            fb.text(WHITE, COL1_LEFT, line3, "head:", 10);
            fb.text(WHITE, COL1_LEFT, line4, "length:", 10);
            fb.text(WHITE, COL2_LEFT, line2, mCursorText.c_str(), 10);
            fb.text(WHITE, COL2_LEFT, line3, mHeadText.c_str(), 10);
            fb.text(WHITE, COL2_LEFT, line4, mDurationText.c_str(), 10);
            break;
        }
    }

    void SampleEditorSubDisplay::setName(const std::string &name)
    {
        mName = name;
    }

    void SampleEditorSubDisplay::setCursorMode()
    {
        mMode = cursorMode;
    }

    void SampleEditorSubDisplay::setFollowMode()
    {
        mMode = followMode;
    }

} /* namespace od */
