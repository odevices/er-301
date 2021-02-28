/*
 * HeadDisplay.cpp
 *
 *  Created on: 23 Nov 2017
 *      Author: clarkson
 */

#include <od/graphics/sampling/TapeHeadDisplay.h>

namespace od
{

    TapeHeadDisplay::TapeHeadDisplay(TapeHead *head, int left, int bottom, int width, int height) : HeadDisplay(head, left, bottom, width, height)
    {
    }

    TapeHeadDisplay::~TapeHeadDisplay()
    {
    }

    void TapeHeadDisplay::resetHead()
    {
        TapeHead *pTapeHead = tapeHead();
        if (pTapeHead)
        {
            pTapeHead->reset();
        }
    }

    void TapeHeadDisplay::drawStatus(FrameBuffer &fb)
    {
        switch (mZoomGadgetState)
        {
        case showTimeGadget:
            fb.text(WHITE, mWorldLeft + 5, mWorldBottom + mHeight - 8,
                    "zooming: time", 10);
            break;
        case showGainGadget:
            fb.text(WHITE, mWorldLeft + 5, mWorldBottom + mHeight - 8,
                    "zooming: height", 10);
            break;
        case gadgetHidden:
            if (isPaused())
            {
                fb.text(WHITE, mWorldLeft + 5, mWorldBottom + mHeight - 8, "paused",
                        10);
            }
            break;
        }
    }

} /* namespace od */
