/*
 * SliceHeadDisplay.cpp
 *
 *  Created on: 18 Nov 2017
 *      Author: clarkson
 */

#include <od/graphics/sampling/SliceHeadDisplay.h>

namespace od
{

    SliceHeadDisplay::SliceHeadDisplay(SliceHead *head, int left, int bottom,
                                       int width, int height) : TapeHeadDisplay(head, left, bottom, width, height)
    {
    }

    SliceHeadDisplay::~SliceHeadDisplay()
    {
    }

    void SliceHeadDisplay::draw(FrameBuffer &fb)
    {
        int playPosition = 0;
        if (mpHead)
        {
            Sample *pSample = mpHead->getSample();
            mSampleView.setSample(pSample);
            playPosition = mpHead->getPosition();
        }

        mSampleView.setCenterPosition(playPosition);
        mSampleView.draw(fb);
        mSampleView.drawPositionOverview(fb, playPosition);
        drawSlices(fb);
        mSampleView.drawPosition(fb, playPosition, "P");
        drawStatus(fb);

        switch (mZoomGadgetState)
        {
        case showTimeGadget:
            mSampleView.drawTimeZoomGadget(fb);
            break;
        case showGainGadget:
            mSampleView.drawGainZoomGadget(fb);
            break;
        case gadgetHidden:

            break;
        }

        mCursorState.copyAttributes(mSampleView.mCursorState);
    }

    void SliceHeadDisplay::drawSlices(FrameBuffer &fb, Slices *pSlices)
    {
        if (mpHead == 0)
            return;

        SliceHead *pSliceHead = (SliceHead *)mpHead;
        Sample *pSample = mpHead->mpSample;

        if (pSample == 0)
            return;

        if (pSlices == 0)
        {
            if (pSliceHead == 0)
                return;

            pSlices = pSliceHead->mpSlices;
            if (pSlices == 0)
                return;
        }

        SlicesIterator i = pSlices->getPositionBefore(mSampleView.mStart);
        SlicesIterator end = pSlices->getPositionAfter(mSampleView.mEnd);
        int y0 = mWorldBottom + mHeight - 1;

        int nVisible = std::distance(i, end);
        float timeVisible = mSampleView.getViewTimeInSeconds();
        bool showHighDetail = nVisible < 16 || timeVisible < 2;
        bool showMediumDetail = nVisible < 32 || timeVisible < 60;

        Color lineColor, headColor;
        if (showHighDetail)
        {
            lineColor = GRAY12;
            headColor = WHITE;
        }
        else if (showMediumDetail)
        {
            lineColor = GRAY9;
            headColor = GRAY12;
        }
        else
        {
            lineColor = GRAY5;
            headColor = GRAY9;
        }

        if (nVisible > 64)
        {
            lineColor = GRAY3;
            headColor = GRAY3;
        }

        while (i < end)
        {
            Slice &slice = *i;
            if (slice.mStart > mSampleView.mEnd)
                break;

            // slice start
            int x0 = mWorldLeft + (int)(mSampleView.mPixelsPerSample * (slice.mStart - mSampleView.mStart));

            if (x0 > mWorldLeft)
            {
                fb.vline(lineColor, x0, mWorldBottom, y0);
                if (showMediumDetail)
                {
                    fb.drawDownTriangle(headColor, x0, y0, 4);
                }
            }

            if (showMediumDetail && slice.hasNonTrivialLoop())
            {
                int L0, L1, h = y0 - 8;
                int mid = mWorldBottom + mHeight / 2;

                // slice loop start
                if (slice.mLoopStart > 0)
                {
                    L0 = mWorldLeft + (int)(mSampleView.mPixelsPerSample * (slice.mLoopStart - mSampleView.mStart));
                }
                else
                {
                    L0 = x0;
                }

                // slice loop end
                if (slice.mLoopEnd > 0)
                {
                    L1 = mWorldLeft + (int)(mSampleView.mPixelsPerSample * (slice.mLoopEnd - mSampleView.mStart));
                }
                else
                {
                    SlicesIterator j = i + 1;
                    if (j < end)
                    {
                        Slice &nextSlice = *j;
                        L1 = mWorldLeft + (int)(mSampleView.mPixelsPerSample * (nextSlice.mStart - mSampleView.mStart));
                    }
                    else
                    {
                        L1 = mWorldLeft + pSample->mSampleCount - mSampleView.mStart;
                    }
                }

                if (showHighDetail)
                {
                    if (L0 > mWorldLeft && L0 < mWorldLeft + mWidth)
                    {
                        fb.vline(lineColor, L0, mid, h);
                        fb.drawRightTriangle(headColor, L0, h, 4);
                    }
                    if (L1 > mWorldLeft && L1 < mWorldLeft + mWidth)
                    {
                        fb.vline(lineColor, L1, mid, h);
                        fb.drawLeftTriangle(headColor, L1, h, 4);
                    }
                }

                L0 = MAX(mWorldLeft, L0);
                L1 = MIN(mWorldLeft + mWidth, L1);
                if (L0 < L1)
                {
                    fb.hline(lineColor, L0, L1, mid);
                }
            }

            i++;
        }

        Slice *slice = pSliceHead->getActiveSlice();
        if (slice && slice->mStart > mSampleView.mStart && slice->mStart < mSampleView.mEnd)
        {
            int x0 = mWorldLeft + (int)(mSampleView.mPixelsPerSample * (slice->mStart - mSampleView.mStart));
            int y0 = mWorldBottom + mHeight - 4;
            fb.vline(GRAY13, x0, mWorldBottom, y0);
            fb.fill(GRAY13, x0 - 2, y0, x0 + 2, y0 + 3);
        }
    }

} /* namespace od */
