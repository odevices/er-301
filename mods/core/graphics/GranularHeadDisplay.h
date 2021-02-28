#pragma once

#include <od/graphics/sampling/HeadDisplay.h>
#include <core/objects/granular/GranularHead.h>

namespace od
{

    class GranularHeadDisplay : public HeadDisplay
    {
    public:
        GranularHeadDisplay(GranularHead *head, int left, int bottom, int width, int height);
        virtual ~GranularHeadDisplay();

#ifndef SWIGLUA
        virtual void draw(FrameBuffer &fb);

        GranularHead *granularHead()
        {
            return (GranularHead *)mpHead;
        }
#endif
    };

} /* namespace od */
