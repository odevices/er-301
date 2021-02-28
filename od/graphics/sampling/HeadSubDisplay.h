/*
 * HeadSubDisplay.h
 *
 *  Created on: 2 Apr 2018
 *      Author: clarkson
 */

#ifndef APP_GRAPHICS_SAMPLING_HEADSUBDISPLAY_H_
#define APP_GRAPHICS_SAMPLING_HEADSUBDISPLAY_H_

#include <od/graphics/Graphic.h>
#include <od/objects/heads/Head.h>

namespace od
{

    class HeadSubDisplay : public Graphic
    {
    public:
        HeadSubDisplay(Head *head);
        virtual ~HeadSubDisplay();

#ifndef SWIGLUA
        virtual void draw(FrameBuffer &fb);
#endif

        void setName(const std::string &name);

    protected:
        Head *mpHead = 0;
        uint32_t mLastHeadPosition = 0;
        float mLastDuration = 0;
        std::string mHeadText{"00:00.000"};
        std::string mDurationText{"00:00.000"};
        std::string mName;
    };

} /* namespace od */

#endif /* APP_GRAPHICS_SAMPLING_HEADSUBDISPLAY_H_ */
