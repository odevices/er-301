/*
 * SliceHeadSubDisplay.cpp
 *
 *  Created on: 21 Nov 2017
 *      Author: clarkson
 */

#include <od/graphics/sampling/SliceHeadSubDisplay.h>
#include <stdio.h>

namespace od
{

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

    static void indexString(int index, std::string &result)
    {
        char tmp[32];
        snprintf(tmp, sizeof(tmp), "Slice #%d", index);
        result = tmp;
    }

    SliceHeadSubDisplay::SliceHeadSubDisplay(SliceHead *head) : Graphic(0, 0, 128, 64)
    {
        mpHead = head;
        if (mpHead)
        {
            mpHead->attach();
        }
    }

    SliceHeadSubDisplay::~SliceHeadSubDisplay()
    {
        if (mpHead)
        {
            mpHead->release();
        }
        mpHead = 0;
    }

    void SliceHeadSubDisplay::draw(FrameBuffer &fb)
    {
        const int line1 = 55;
        const int line2 = 42;
        const int line3 = 32;
        const int line4 = 22;
        const int col1 = 5;
        const int col2 = 38;
        const int col3 = 83;
        const int col4 = 86;
        uint32_t n;

        Graphic::draw(fb);

        if (mpHead == 0)
            return;

        Sample *pSample = mpHead->getSample();

        if (pSample == 0)
        {
            fb.text(WHITE, col1, line1, "No sample.", 10);
            fb.text(WHITE, col1, line3, "head:", 10);
            fb.text(WHITE, col1, line4, "len:", 10);
            fb.text(WHITE, col2, line2, "00:00.000", 10);
            fb.text(WHITE, col2, line3, "00:00.000", 10);
            fb.text(WHITE, col2, line4, "00:00.000", 10);
            return;
        }

        float sampleRate = pSample->mSampleRate;
        n = mpHead->getPosition();
        if (n != mLastHeadPosition)
        {
            mLastHeadPosition = n;
            float totalSecs = ((double)n) / (double)sampleRate;
            timeString(totalSecs, mHeadText);
        }

        // active slice
        Slice *slice = mpHead->getActiveSlice();
        if (slice)
        {
            n = slice->mStart;
            if (n != mLastSlicePosition)
            {
                mLastSlicePosition = n;
                float totalSecs = ((double)n) / (double)sampleRate;
                timeString(totalSecs, mSliceText);
                indexString(mpHead->mActiveSliceIndex, mSliceIndexText);
            }
        }

        if (pSample->mTotalSeconds != mLastDuration)
        {
            mLastDuration = pSample->mTotalSeconds;
            timeString(mLastDuration, mDurationText);
        }

        fb.text(WHITE, col1, line1, mName.c_str(), 10);
        fb.text(WHITE, col1, line3, "head:", 10);
        fb.text(WHITE, col1, line4, "len:", 10);
        fb.text(WHITE, col2, line3, mHeadText.c_str(), 10);
        fb.text(WHITE, col2, line4, mDurationText.c_str(), 10);

        fb.text(WHITE, col4, line2, mSliceIndexText.c_str(), 10);
        fb.text(WHITE, col4, line3, mSliceText.c_str(), 10);

        fb.vline(WHITE, col3, 0, 52);
        fb.hline(WHITE, 0, 128, 52);
    }

    void SliceHeadSubDisplay::setName(const std::string &name)
    {
        mName = name;
    }

} /* namespace od */
