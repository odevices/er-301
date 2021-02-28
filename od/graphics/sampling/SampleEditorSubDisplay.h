/*
 * SampleEditorSubDisplay.h
 *
 *  Created on: 31 Mar 2018
 *      Author: clarkson
 */

#ifndef APP_GRAPHICS_SAMPLING_SAMPLEEDITORSUBDISPLAY_H_
#define APP_GRAPHICS_SAMPLING_SAMPLEEDITORSUBDISPLAY_H_

#include <od/graphics/Graphic.h>
#include <od/graphics/sampling/SampleEditorMainDisplay.h>

namespace od
{

    class SampleEditorSubDisplay : public Graphic
    {
    public:
        SampleEditorSubDisplay(SampleEditorMainDisplay *display);
        virtual ~SampleEditorSubDisplay();

#ifndef SWIGLUA
        virtual void draw(FrameBuffer &fb);
#endif
        void setCursorMode();
        void setFollowMode();

        void setName(const std::string &name);

    protected:
        SampleEditorMainDisplay *mpMainDisplay = 0;
        uint32_t mLastHeadPosition = 0;
        uint32_t mLastCursorPosition = 0;
        float mLastDuration = 0;
        std::string mHeadText{"00:00.000"};
        std::string mCursorText{"00:00.000"};
        std::string mDurationText{"00:00.000"};
        std::string mName;

        enum Modes
        {
            cursorMode,
            followMode
        };

        Modes mMode = cursorMode;
    };

} /* namespace od */

#endif /* APP_GRAPHICS_SAMPLING_SAMPLEEDITORSUBDISPLAY_H_ */
