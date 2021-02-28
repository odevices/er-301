#pragma once

#include <od/graphics/Graphic.h>
#include <core/objects/pitch/ScaleQuantizer.h>

namespace od
{

    class PitchCircle : public Graphic
    {
    public:
        PitchCircle(int left, int bottom, int width, int height);
        virtual ~PitchCircle();

#ifndef SWIGLUA
        virtual void draw(FrameBuffer &fb);
#endif

        void setQuantizer(ScaleQuantizer *pQuantizer);

    private:
        int mMargin = 2;
        ScaleQuantizer *mpQuantizer = 0;
    };

} /* namespace od */
