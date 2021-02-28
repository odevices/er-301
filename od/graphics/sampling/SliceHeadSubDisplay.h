/*
 * SliceHeadSubDisplay.h
 *
 *  Created on: 21 Nov 2017
 *      Author: clarkson
 */

#ifndef APP_GRAPHICS_SAMPLING_SLICEHEADSUBDISPLAY_H_
#define APP_GRAPHICS_SAMPLING_SLICEHEADSUBDISPLAY_H_

#include <od/graphics/Graphic.h>
#include <od/objects/heads/SliceHead.h>

namespace od
{

    class SliceHeadSubDisplay : public Graphic
    {
    public:
        SliceHeadSubDisplay(SliceHead *head);
        virtual ~SliceHeadSubDisplay();

#ifndef SWIGLUA
        virtual void draw(FrameBuffer &fb);
#endif

        void setName(const std::string &name);

    protected:
        SliceHead *mpHead = 0;
        uint32_t mLastHeadPosition = 0;
        float mLastDuration = 0;
        std::string mHeadText{"00:00.000"};
        std::string mDurationText{"00:00.000"};
        std::string mName;

        uint32_t mLastSlicePosition = 0;
        std::string mSliceText{"00:00.000"};
        std::string mSliceIndexText{"No slice."};
    };

} /* namespace od */

#endif /* APP_GRAPHICS_SAMPLING_SLICEHEADSUBDISPLAY_H_ */
