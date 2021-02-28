/*
 * RecordHeadDisplay.h
 *
 *  Created on: 18 Nov 2017
 *      Author: clarkson
 */

#ifndef APP_GRAPHICS_RECORDHEADDISPLAY_H_
#define APP_GRAPHICS_RECORDHEADDISPLAY_H_

#include <od/objects/heads/RecordHead.h>
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

#endif /* APP_GRAPHICS_RECORDHEADDISPLAY_H_ */
