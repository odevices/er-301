/*
 * StateMachineView.h
 *
 *  Created on: 18 Jan 2018
 *      Author: clarkson
 */

#ifndef APP_GRAPHICS_CONTROLS_STATEMACHINEVIEW_H_
#define APP_GRAPHICS_CONTROLS_STATEMACHINEVIEW_H_

#include <od/graphics/Graphic.h>
#include <od/graphics/controls/SoftButton.h>
#include <od/extras/LinearRamp.h>
#include <od/objects/StateMachine.h>
#include "../../objects/timing/Comparator.h"

namespace od
{

    class StateMachineView : public Graphic
    {
    public:
        StateMachineView(int left, int bottom, int width, int height, StateMachine &stateMachine, Comparator &edge);
        virtual ~StateMachineView();

#ifndef SWIGLUA
        virtual void draw(FrameBuffer &fb);
#endif

        void setLabel(const std::string &text);
        void showStateValue()
        {
            mShowStateValue = true;
        }

    private:
        StateMachine &mStateMachine;
        SoftButton mNameLabel{" ---- "};
        Comparator &mEdgeDetector;
        std::string mCurrentStateName;
        std::string mNextStateName;
        int mCurrentIndex = -1;
        int mLastCount = 0;
        LinearRamp mFade;
        int mShowStateValue = false;
    };

} /* namespace od */

#endif /* APP_GRAPHICS_CONTROLS_STATEMACHINEVIEW_H_ */
