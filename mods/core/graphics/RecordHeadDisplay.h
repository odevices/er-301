#pragma once
#include <core/objects/heads/RecordHead.h>
#include <od/graphics/sampling/TapeHeadDisplay.h>

namespace od
{

    class RecordHeadDisplay : public TapeHeadDisplay
    {
    public:
        RecordHeadDisplay(RecordHead *head, int left, int bottom, int width, int height);
        virtual ~RecordHeadDisplay();

#ifndef SWIGLUA
        virtual void draw(FrameBuffer &fb);
        void drawStatus(FrameBuffer &fb);

        RecordHead *recordHead()
        {
            return (RecordHead *)mpHead;
        }

        uint32_t mLastRecordPosition = 0;
#endif
    };

} /* namespace od */

