#include <od/graphics/sampling/HeadDisplay.h>

namespace od
{

    HeadDisplay::HeadDisplay(Head *head, int left, int bottom, int width, int height) : Graphic(left, bottom, width, height), mSampleView(left, bottom, width,
                                                                                                                                          height)
    {
        own(mSampleView);
        addChild(&mSampleView);
        mpHead = head;
        if (mpHead)
        {
            mpHead->attach();
            mSampleView.setSample(mpHead->getSample());
        }
        setPointerLabel("P");
    }

    HeadDisplay::~HeadDisplay()
    {
        if (mpHead)
        {
            mpHead->release();
        }
    }

    void HeadDisplay::draw(FrameBuffer &fb)
    {
        uint32_t pos = 0;
        if (mpHead)
        {
            Sample *pSample = mpHead->getSample();
            pos = mpHead->getPosition();

            if (mSampleView.setSample(pSample))
            {
                // new sample, so do nothing
            }
        }

        mSampleView.setCenterPosition(pos);
        mSampleView.draw(fb);
        mSampleView.drawPositionOverview(fb, pos);
        mSampleView.drawPosition(fb, pos, mPointerLabel.c_str(), 6);
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

    bool HeadDisplay::encoderZoom(int change, bool shifted, int threshold)
    {
        switch (mZoomGadgetState)
        {
        case showTimeGadget:
            return mSampleView.encoderZoomTime(change, shifted, threshold);
        case showGainGadget:
            return mSampleView.encoderZoomGain(change, shifted, threshold);
        default:
            return false;
        }
    }

    void HeadDisplay::notifyVisible()
    {
        Graphic::notifyVisible();
        mSampleView.notifyVisible();
    }

    void HeadDisplay::drawStatus(FrameBuffer &fb)
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
            break;
        }
    }

} /* namespace od */
