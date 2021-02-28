/*
 * StateView.cpp
 *
 *  Created on: 18 Jan 2018
 *      Author: clarkson
 */

#include <od/graphics/controls/StateMachineView.h>
#include <stdio.h>

#define HATPITCH 5

namespace od
{

    StateMachineView::StateMachineView(int left, int bottom, int width, int height,
                                       StateMachine &stateMachine,
                                       Comparator &edge) : Graphic(left, bottom, width, height), mStateMachine(stateMachine), mEdgeDetector(edge)
    {
        mStateMachine.attach();
        mEdgeDetector.attach();
        own(mNameLabel);
        addChild(&mNameLabel);
        mNameLabel.setCenter(mWidth / 2);
        mFade.setLength(GRAPHICS_REFRESH_RATE / 2);
        mFade.mValue = 2;
        mCursorState.orientation = cursorRight;
    }

    StateMachineView::~StateMachineView()
    {
        mStateMachine.release();
        mEdgeDetector.release();
    }

    void StateMachineView::setLabel(const std::string &text)
    {
        mNameLabel.setText(text);
        mNameLabel.setCenter(mWidth / 2);
    }

    void StateMachineView::draw(FrameBuffer &fb)
    {
        if (mCurrentIndex != mStateMachine.getCurrentStateIndex())
        {
            mCurrentIndex = mStateMachine.getCurrentStateIndex();
            mCurrentStateName = mStateMachine.getStateName(
                mStateMachine.getCurrentStateIndex());
            mNextStateName = mStateMachine.getStateName(
                mStateMachine.getNextStateIndex());
            setLabel(mNextStateName);
        }

        int mid = mWorldLeft + mWidth / 2;
        int left = mid - mWidth / 4;
        int right = mid + mWidth / 4;
        int height = mHeight / 4;
        int bottom = mWorldBottom + mHeight / 2 - height / 2 - HATPITCH / 2;
        int top = mWorldBottom + mHeight / 2 + height / 2 - HATPITCH / 2;
        int edgeCount = mEdgeDetector.getRisingEdgeCount();
        // set the cursor
        mCursorState.x = mWorldLeft;
        mCursorState.y = mWorldBottom + GRID4_LINE2;

        if (edgeCount > mLastCount)
        {
            mFade.reset(-1, 2);
        }
        else
        {
            mFade.step();
        }
        mLastCount = edgeCount;

        Graphic::draw(fb);

        fb.line(GRAY3, left, bottom, mid, bottom + HATPITCH);
        fb.line(GRAY3, mid, bottom + HATPITCH, right, bottom);
        fb.line(GRAY3, left, top, mid, top + HATPITCH);
        fb.line(GRAY3, mid, top + HATPITCH, right, top);
        fb.vline(GRAY3, left, bottom, top);
        fb.vline(GRAY3, right, bottom, top);

        if (!mFade.done())
        {
            float factor = 1.0f / height;
            for (int i = 0; i < height; i += 2)
            {
                Color color = WHITE * (i * factor - mFade.mValue);
                if (color > 0 && color <= WHITE)
                {
                    fb.line(color, left, bottom + i, mid, bottom + HATPITCH + i);
                    fb.line(color, mid, bottom + HATPITCH + i, right, bottom + i);
                }
            }
        }

        int textWidth, textHeight;

        //getTextSize(mNextStateName.c_str(), &textWidth, &textHeight, 10);
        //fb.text(GRAY11, mWorldLeft + (mWidth - textWidth) / 2, bottom - 5,
        //        mNextStateName.c_str(), 10, ALIGN_MIDDLE);

        if (mShowStateValue)
        {
            char text[64];
            int value = mStateMachine.getValue(mStateMachine.mCurrentIndex);
            snprintf(text, sizeof(text), "#%d", value);
            getTextSize(text, &textWidth, &textHeight, 10);
            fb.text(WHITE, mWorldLeft + (mWidth - textWidth) / 2, top + 15, text,
                    10, ALIGN_MIDDLE);
        }
        else
        {
            getTextSize(mCurrentStateName.c_str(), &textWidth, &textHeight, 10);
            fb.text(WHITE, mWorldLeft + (mWidth - textWidth) / 2, top + 15,
                    mCurrentStateName.c_str(), 10, ALIGN_MIDDLE);
        }
    }

} /* namespace od */
