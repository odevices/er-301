/*
 * SliceHeadDisplay.h
 *
 *  Created on: 18 Nov 2017
 *      Author: clarkson
 */

#ifndef APP_GRAPHICS_SLICEHEADDISPLAY_H_
#define APP_GRAPHICS_SLICEHEADDISPLAY_H_

#include <od/objects/heads/SliceHead.h>
#include <od/graphics/sampling/TapeHeadDisplay.h>

namespace od
{

    class SliceHeadDisplay : public TapeHeadDisplay
    {
    public:
        SliceHeadDisplay(SliceHead *head, int left, int bottom, int width, int height);
        virtual ~SliceHeadDisplay();

#ifndef SWIGLUA
        virtual void draw(FrameBuffer &fb);
        void drawSlices(FrameBuffer &fb, Slices *pSlices = 0);

        SliceHead *sliceHead()
        {
            return (SliceHead *)tapeHead();
        }

        Slices *slices()
        {
            return sliceHead()->mpSlices;
        }
#endif
    };

} /* namespace od */

#endif /* APP_GRAPHICS_SLICEHEADDISPLAY_H_ */
