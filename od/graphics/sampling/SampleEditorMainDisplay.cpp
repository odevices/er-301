/*
 * SampleEditorMainDisplay.cpp
 *
 *  Created on: 29 Mar 2018
 *      Author: clarkson
 */

#include <od/graphics/sampling/SampleEditorMainDisplay.h>

namespace od
{

    SampleEditorMainDisplay::SampleEditorMainDisplay(Head *head) : HeadDisplay(head, 0, 0, 256, 64)
    {
        setPointerLabel("P");
    }

    SampleEditorMainDisplay::~SampleEditorMainDisplay()
    {
    }

    bool SampleEditorMainDisplay::encoderPointer(int change, bool shifted,
                                                 float sensitivity)
    {
        if (mMarkState == 1)
        {
            mMarkState = 2;
            mAnchor = mPendingAnchor;
        }
        change = mEncoderHysteresis.onChange(change);
        movePointerByProportion(change * sensitivity);
        if (mMarkState == 2)
        {
            if (mPointer > mAnchor)
            {
                mSampleView.setMarkedRegion(mAnchor, mPointer);
            }
            else
            {
                mSampleView.setMarkedRegion(mPointer, mAnchor);
            }
        }
        return true;
    }

    void SampleEditorMainDisplay::draw(FrameBuffer &fb)
    {
        int playPosition = 0;

        if (mpHead)
        {
            Sample *pSample = mpHead->getSample();
            if (mSampleView.setSample(pSample))
            {
                // Sample changed, notify
                mPointer = (mSampleView.mStart + mSampleView.mEnd) / 2;
            }
            playPosition = mpHead->getPosition();
        }

        if (mFollowHead)
        {
            mPointer = playPosition;
        }

        mSampleView.setCenterPosition(mPointer);
        mSampleView.draw(fb);
        mSampleView.drawPositionOverview(fb, playPosition);

        mSampleView.drawPosition(fb, playPosition, mPointerLabel.c_str());

        // draw pointer
        if (mPointer > mSampleView.mEnd)
        {
            mCursorState.x = mWorldLeft + mWidth - CURSOR_HEIGHT;
            mCursorState.y = mWorldBottom + mHeight / 2;
            mCursorState.orientation = CursorOrientation::cursorRight;
        }
        else if (mPointer < mSampleView.mStart)
        {
            mCursorState.x = mWorldLeft + CURSOR_HEIGHT;
            mCursorState.y = mWorldBottom + mHeight / 2;
            mCursorState.orientation = CursorOrientation::cursorLeft;
        }
        else
        {
            // pointer is visible
            int x0 = mWorldLeft + (int)(mSampleView.mPixelsPerSample * (mPointer - mSampleView.mStart));
            int y0 = mWorldBottom + mHeight - 3;
            fb.vline(WHITE, x0, mWorldBottom, y0);
            fb.drawDownTriangle(WHITE, x0, y0, 4);
            mCursorState.x = x0;
            mCursorState.y = y0;
            mCursorState.orientation = CursorOrientation::cursorDown;
        }

        switch (mZoomGadgetState)
        {
        case showTimeGadget:
            mSampleView.drawTimeZoomGadget(fb);
            mCursorState.copyAttributes(mSampleView.mCursorState);
            break;
        case showGainGadget:
            mSampleView.drawGainZoomGadget(fb);
            mCursorState.copyAttributes(mSampleView.mCursorState);
            break;
        case gadgetHidden:
            break;
        }

        drawStatus(fb);
    }

    int SampleEditorMainDisplay::findNearestZeroCrossing(Sample *pSample, int pos,
                                                         int windowStart,
                                                         int windowEnd)
    {
        if (pSample == 0)
        {
            return pos;
        }

        windowStart = MIN(windowStart, (int)pSample->mSampleCount);
        windowEnd = MIN(windowEnd, (int)pSample->mSampleCount);

        if (windowStart > pos || windowEnd < pos)
            return pos;

        int i, j0, j1, n, ch;

        ch = pSample->mChannelCount;

        bool found = false;
        float *data = pSample->mpData + pos * ch;
        // search forward
        n = windowEnd - pos;
        n--;
        n *= ch;
        for (i = 0, j0 = 0; i < n; i += ch, j0++)
        {
            if (data[i] * data[i + ch] <= 0)
            {
                found = true;
                break;
            }
        }

        // search backward
        n = pos - windowStart;
        n--;
        n *= ch;
        for (i = 0, j1 = 0; i < n; i += ch, j1++)
        {
            if (data[-i] * data[-i - ch] <= 0)
            {
                found = true;
                break;
            }
        }

        if (found)
        {
            if (j0 < j1)
            {
                pos += j0;
            }
            else
            {
                pos -= j1;
            }
        }

        return pos;
    }

    int SampleEditorMainDisplay::movePointerToNearestZeroCrossing()
    {
        if (mpHead == 0)
            return 0;

        Sample *pSample = mpHead->getSample();

        if (pSample == 0)
            return 0;

        int save = mPointer;
        mPointer = findNearestZeroCrossing(pSample, mPointer, mSampleView.mStart,
                                           mSampleView.mStart + mSampleView.mEnd);

        if (mMarkState == 2)
        {
            if (mPointer > mAnchor)
            {
                mSampleView.setMarkedRegion(mAnchor, mPointer);
            }
            else
            {
                mSampleView.setMarkedRegion(mPointer, mAnchor);
            }
        }
        return (int)mPointer - save;
    }

    void SampleEditorMainDisplay::movePointerByProportion(float amount)
    {
        int w = mSampleView.mSamplesPerPixel * mWidth;
        movePointerInSamples((int)(w * amount));
    }

    void SampleEditorMainDisplay::movePointerInSamples(int amount)
    {
        if (mpHead == 0)
            return;

        Sample *pSample = mpHead->getSample();

        if (pSample == 0)
            return;

        if (amount > 0)
        {
            int amt = (int)amount;
            int remaining = pSample->mSampleCount - mPointer - 1;
            if (amt < remaining)
            {
                mPointer += amt;
            }
            else
            {
                mPointer = pSample->mSampleCount - 1;
            }
        }
        else if (amount < 0)
        {
            int amt = (int)(-amount);
            int remaining = mPointer;
            if (amt < remaining)
            {
                mPointer -= amt;
            }
            else
            {
                mPointer = 0;
            }
        }
    }

    void SampleEditorMainDisplay::movePointerToHead()
    {
        if (mpHead)
        {
            mPointer = mpHead->getPosition();
        }
    }

    void SampleEditorMainDisplay::movePointerToViewCenter()
    {
        mPointer = (mSampleView.mStart + mSampleView.mEnd) / 2;
    }

    void SampleEditorMainDisplay::beginMarking()
    {
        mPendingAnchor = mPointer;
        mMarkState = 1;
    }

    void SampleEditorMainDisplay::endMarking()
    {
        mMarkState = 0;
    }

    void SampleEditorMainDisplay::clearMarking()
    {
        mSampleView.setMarkedRegion(0, 0);
        mMarkState = 0;
    }

    bool SampleEditorMainDisplay::isMarked()
    {
        return mSampleView.mMarkStart < mSampleView.mMarkEnd;
    }

    float SampleEditorMainDisplay::getMarkedDuration()
    {
        if (mpHead == 0)
            return 0;

        Sample *pSample = mpHead->getSample();

        if (pSample == 0)
            return 0;

        return (mSampleView.mMarkEnd - mSampleView.mMarkStart) / pSample->mSampleRate;
    }

    float SampleEditorMainDisplay::getMarkedBeginTime()
    {
        if (mpHead == 0)
            return 0;

        Sample *pSample = mpHead->getSample();

        if (pSample == 0)
            return 0;

        return mSampleView.mMarkStart / pSample->mSampleRate;
    }

    float SampleEditorMainDisplay::getMarkedEndTime()
    {
        if (mpHead == 0)
            return 0;

        Sample *pSample = mpHead->getSample();

        if (pSample == 0)
            return 0;

        return mSampleView.mMarkEnd / pSample->mSampleRate;
    }

} /* namespace od */
