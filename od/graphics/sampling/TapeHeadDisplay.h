#pragma once

#include <od/graphics/sampling/HeadDisplay.h>
#include <od/objects/heads/TapeHead.h>

namespace od
{

    class TapeHeadDisplay : public HeadDisplay
    {
    public:
        TapeHeadDisplay(TapeHead *head, int left, int bottom, int width, int height);
        virtual ~TapeHeadDisplay();

        void resetHead();
        bool isPaused()
        {
            return tapeHead() && tapeHead()->mPaused;
        }

#ifndef SWIGLUA
        void drawStatus(FrameBuffer &fb);

        TapeHead *tapeHead()
        {
            return (TapeHead *)mpHead;
        }
#endif
    };

} /* namespace od */
