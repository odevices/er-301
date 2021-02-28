/*
 * RecordHeadDisplay.cpp
 *
 *  Created on: 18 Nov 2017
 *      Author: clarkson
 */

#include <od/graphics/sampling/RecordHeadDisplay.h>

namespace od
{

    RecordHeadDisplay::RecordHeadDisplay(RecordHead *head, int left, int bottom, int width,
                                         int height) : TapeHeadDisplay(head, left, bottom, width, height)
    {
    }

    RecordHeadDisplay::~RecordHeadDisplay()
    {
    }

    void RecordHeadDisplay::draw(FrameBuffer &fb)
    {
        uint32_t pos = 0;
        RecordHead *pRecordHead = recordHead();
        if (pRecordHead)
        {
            Sample *pSample = pRecordHead->getSample();
            pos = pRecordHead->getPosition();

            if (mSampleView.setSample(pSample))
            {
                // new sample, so do nothing
            }
            else if (mLastRecordPosition != pos)
            {
                if (mLastRecordPosition < pos)
                {
                    mSampleView.invalidateInterval(mLastRecordPosition, pos);
                }
                else
                {
                    // wrapped
                    mSampleView.invalidateInterval(mLastRecordPosition, pSample->mSampleCount);
                    mSampleView.invalidateInterval(0, pos);
                }
            }

            mLastRecordPosition = pos;
        }

        mSampleView.setCenterPosition(pos);
        mSampleView.draw(fb);
        mSampleView.drawPositionOverview(fb, pos);
        mSampleView.drawPosition(fb, pos, "R", 6);
        drawStatus(fb);

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
        default:
            break;
        }
    }

    void RecordHeadDisplay::drawStatus(FrameBuffer &fb)
    {
        // draw play head status
        if (isPaused())
        {
            fb.text(WHITE, mWorldLeft + 5, mWorldBottom + mHeight - 8, "paused",
                    10);
        }
    }

} /* namespace od */
