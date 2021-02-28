#pragma once

#include <od/graphics/Graphic.h>
#include <od/graphics/sampling/SampleView.h>
#include <od/objects/heads/Head.h>

namespace od
{

    class HeadDisplay : public Graphic
    {
    public:
        HeadDisplay(Head *head, int left, int bottom, int width, int height);
        virtual ~HeadDisplay();

        bool encoderZoom(int change, bool shifted, int threshold);

        void showTimeZoomGadget()
        {
            mZoomGadgetState = showTimeGadget;
        }

        void showGainZoomGadget()
        {
            mZoomGadgetState = showGainGadget;
        }

        void hideZoomGadget()
        {
            mZoomGadgetState = gadgetHidden;
        }

        void forceRefresh()
        {
            mSampleView.forceRefresh();
        }

        void setPointerLabel(const char *label)
        {
            mPointerLabel = label;
        }

        float getViewTimeInSeconds()
        {
            return mSampleView.getViewTimeInSeconds();
        }

        void setViewTimeInSeconds(float secs)
        {
            mSampleView.setViewTimeInSeconds(secs);
        }

        int getViewGainInDecibels()
        {
            return mSampleView.getViewGainInDecibels();
        }

        void setViewGainInDecibels(int decibels)
        {
            mSampleView.setViewGainInDecibels(decibels);
        }

        void setChannel(int channel)
        {
            mSampleView.setChannel(channel);
        }

#ifndef SWIGLUA
        virtual void draw(FrameBuffer &fb);
        virtual void notifyVisible();
        void drawStatus(FrameBuffer &fb);

        Head *head()
        {
            return mpHead;
        }

        SampleView mSampleView;
        enum ZoomGadgetState
        {
            gadgetHidden,
            showTimeGadget,
            showGainGadget
        };
        ZoomGadgetState mZoomGadgetState = gadgetHidden;
#endif

    protected:
        Head *mpHead = 0;
        std::string mPointerLabel;
    };

} /* namespace od */
