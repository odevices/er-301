#include <core/graphics/GranularHeadDisplay.h>

namespace od
{

    GranularHeadDisplay::GranularHeadDisplay(GranularHead *head, int left,
                                             int bottom, int width, int height) : HeadDisplay(head, left, bottom, width, height)
    {
        mSampleView.setMarkVisible(true);
        mSampleView.setMarkColor(GRAY7);
        setPointerLabel("G");
    }

    GranularHeadDisplay::~GranularHeadDisplay()
    {
    }

    void GranularHeadDisplay::draw(FrameBuffer &fb)
    {
        GranularHead *pGranularHead = granularHead();

        if (pGranularHead == 0)
        {
            return;
        }

        Sample *pSample = pGranularHead->getSample();

        if (pSample == 0)
        {
            HeadDisplay::draw(fb);
            return;
        }

        int G = pGranularHead->getGrainCount();
        int H = mHeight / 2;

        float speed = pGranularHead->mSpeed.buffer()[0];
        float duration = pGranularHead->mDuration.value();
        int L = duration * speed * pSample->mSampleRate;
        int pos = pGranularHead->mCurrentIndex;

        if (L > 0)
        {
            mSampleView.setMarkedRegion(pos, pos + L);
        }
        else
        {
            mSampleView.setMarkedRegion(pos + L, pos);
        }

        HeadDisplay::draw(fb);

        for (int i = 0; i < G; i++)
        {
            Grain *grain = pGranularHead->getGrain(i);
            if (grain == 0)
                break;
            if (grain->mActive)
            {
                int h = grain->mLastEnvelopeValue * H;
                mSampleView.drawMiniPosition(fb, grain->mCurrentIndex, h, 0);
            }
        }
    }

} /* namespace od */
