/*
 * TriggerGroup.h
 *
 *  Created on: 29 May 2019
 *      Author: clarkson
 */

#ifndef APP_GRAPHICS_HOLDVIEW_TRIGGERGROUP_H_
#define APP_GRAPHICS_HOLDVIEW_TRIGGERGROUP_H_

#include <od/graphics/Graphic.h>
#include <od/graphics/controls/SoftButton.h>
#include <od/objects/timing/Comparator.h>
#include <vector>

namespace od
{

    class TriggerGroup : public Graphic
    {
    public:
        TriggerGroup();
        virtual ~TriggerGroup();

#ifndef SWIGLUA
        virtual void draw(FrameBuffer &fb);
#endif

        void setLabel(const std::string &text);
        const std::string &getLabel();
        void setLabelCenter(int x);
        void showLabel();
        void hideLabel();

        void onPressed();
        void onReleased();

        void add(Comparator *o);
        void remove(Comparator *o);
        void clear();
        int size();

    protected:
        std::vector<Comparator *> mComparators;
        SoftButton mNameLabel{" ---- ", 1};
        int mFill = 0;
        bool mPressed = false;
    };

} /* namespace od */

#endif /* APP_GRAPHICS_HOLDVIEW_TRIGGERGROUP_H_ */
