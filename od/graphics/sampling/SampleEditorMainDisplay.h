#pragma once

#include <od/graphics/sampling/HeadDisplay.h>
#include <od/ui/EncoderHysteresis.h>

namespace od
{

    class SampleEditorMainDisplay : public HeadDisplay
    {
    public:
        SampleEditorMainDisplay(Head *head);
        virtual ~SampleEditorMainDisplay();

        void movePointerByProportion(float amount);
        void movePointerInSamples(int amount);
        void movePointerToHead();
        int movePointerToNearestZeroCrossing();
        void movePointerToViewCenter();
        int getPointer()
        {
            return mPointer;
        }

        void beginMarking();
        void endMarking();
        void clearMarking();
        bool isMarked();
        float getMarkedDuration();
        float getMarkedBeginTime();
        float getMarkedEndTime();
        int getMarkStart()
        {
            return mSampleView.mMarkStart;
        }
        int getMarkEnd()
        {
            return mSampleView.mMarkEnd;
        }

        void enableFollowing()
        {
            mFollowHead = true;
        }

        void disableFollowing()
        {
            mFollowHead = false;
        }

        bool isFollowing()
        {
            return mFollowHead;
        }

        bool encoderPointer(int change, bool shifted, float sensitivity);

        void forceRefresh()
        {
            mSampleView.forceRefresh();
        }

#ifndef SWIGLUA
        virtual void draw(FrameBuffer &fb);

        // pointer
        EncoderHysteresis mEncoderHysteresis;
        int mPointer = 0;
        int mAnchor = 0;
        bool mFollowHead = false;
        int mPendingAnchor = 0;
        int mMarkState = 0; // 0 - not marking, 1 - waiting for movement, 2 - marking
#endif

    private:
        int findNearestZeroCrossing(Sample *pSample, int pos, int windowStart,
                                    int windowEnd);
    };

} /* namespace od */
