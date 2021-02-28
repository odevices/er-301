/*
 * DurationControl.h
 *
 *  Created on: 8 Sep 2016
 *      Author: clarkson
 */

#ifndef APP_GRAPHICS_DURATIONCONTROL_H_
#define APP_GRAPHICS_DURATIONCONTROL_H_

#include <od/graphics/Graphic.h>
#include <string>

namespace od
{

    enum DurationMode
    {
        durationMMSSXXX,
        durationSSXXX,
        durationBeats
    };

    class DurationControl : public Graphic
    {
    public:
        DurationControl();
        virtual ~DurationControl();

#ifndef SWIGLUA
        virtual void draw(FrameBuffer &fb);
#endif

        void setMode(DurationMode mode)
        {
            mMode = mode;
            mDisplayedValue = -1;
        }

        inline int getBeats()
        {
            return (int)(mSeconds * mBaseTempo / 60.0f + 0.5f);
        }

        inline float beatsToSeconds(int beats)
        {
            return 60.0f * beats / mBaseTempo;
        }

        void setBaseTempo(int bpm)
        {
            if (bpm < 1)
                bpm = 1;
            else if (bpm > 999)
            {
                bpm = 999;
            }
            int beats = getBeats();
            mBaseTempo = bpm;
            mSeconds = beatsToSeconds(beats);
        }

        float getTotalSeconds()
        {
            return mSeconds;
        }

        void setTotalSeconds(float secs)
        {
            if (secs < 0.0f)
            {
                secs = 0.0f;
            }
            else if (secs > 3599.0f)
            {
                secs = 3599.0f;
            }
            mSeconds = secs;
        }

        void increment();
        void decrement();
        bool encoder(int change, bool shifted, int threshold);
        void setEncoderFocus(int focus);

        void setTextSize(int size)
        {
            mTextSize = size;
        }

    private:
        float mSeconds = 0;
        float mDisplayedValue = -1;
        std::string mDisplayText;
        DurationMode mMode = durationMMSSXXX;
        int mTextSize = 12;
        int mTextWidth, mTextHeight;
        int mBaseTempo = 120;
        int mEncoderFocus = 0;
        int mEncoderSum = 0;
    };

} /* namespace od */

#endif /* APP_GRAPHICS_DURATIONCONTROL_H_ */
