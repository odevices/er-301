#pragma once

#include <od/objects/Object.h>
#include <od/audio/Sample.h>
#include <od/config.h>

namespace od
{

    class Head : public Object
    {
    public:
        Head();
        virtual ~Head();

        virtual void setSample(Sample *sample);
        Sample *getSample()
        {
            return mpSample;
        }

        void setPosition(int index)
        {
            mCurrentIndex = index;
        }

        int getPosition()
        {
            return mCurrentIndex;
        }

        float getSampleRate()
        {
            if (mpSample)
            {
                return mpSample->mSampleRate;
            }
            else
            {
                return globalConfig.sampleRate;
            }
        }

#ifndef SWIGLUA
        Sample *mpSample = NULL;

        Parameter mSampleDuration{"Sample Duration"};

        // current position
        int mCurrentIndex = 0;
        int mEndIndex = 0;
#endif
    };

} /* namespace od */
