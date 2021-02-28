/*
 * GraphicContext.cpp
 *
 *  Created on: 10 Mar 2017
 *      Author: clarkson
 */

#include <od/graphics/GraphicContext.h>

namespace od
{

    GraphicContext::GraphicContext()
    {
    }

    GraphicContext::~GraphicContext()
    {
        clear();
    }

    void GraphicContext::clear()
    {
        slideUpTo(0, 0);
        setOverlayGraphic(0);
        setCursorController(0);
    }

    void GraphicContext::draw(FrameBuffer &fb)
    {

        if (mpOldGraphic)
        {
            mpOldGraphic->setPosition(mpOldGraphic->mLeft, mSlidingOldPosition);
            mpOldGraphic->draw(fb);
            if (mpGraphic)
            {
                mpGraphic->setPosition(mpGraphic->mLeft, mSlidingPosition);
                mpGraphic->draw(fb);
            }

            // update sliding position
            mSlidingPosition += mSlidingStep;
            mSlidingOldPosition += mSlidingStep;
            if (mSlidingPosition == 0)
            {
                // done sliding, release old graphic
                mpOldGraphic->setPosition(mpOldGraphic->mLeft, 0);
                if (mpOldGraphic->mVisibility == visibleState)
                {
                    mpOldGraphic->mVisibility = hiddenState;
                    mpOldGraphic->notifyHidden();
                }
                mpOldGraphic->release();
                mpOldGraphic = 0;
                if (mpGraphic)
                {
                    mpGraphic->setPosition(mpGraphic->mLeft, 0);
                }
            }
        }
        else if (mpGraphic)
        {
            if (mpGraphic->visible(fb))
            {
                if (mpGraphic->mVisibility == hiddenState)
                {
                    mpGraphic->mVisibility = visibleState;
                    mpGraphic->notifyVisible();
                }
                mpGraphic->draw(fb);
            }
            else
            {
                if (mpGraphic->mVisibility == visibleState)
                {
                    mpGraphic->mVisibility = hiddenState;
                    mpGraphic->notifyHidden();
                }
            }
        }

        if (mpCursorController && mShowCursor)
        {
            mCursor.draw(mpCursorController->mCursorState, fb);
        }

        if (mpOverlayGraphic)
        {
            if (mpOverlayGraphic->visible(fb))
            {
                if (mpOverlayGraphic->mVisibility == hiddenState)
                {
                    mpOverlayGraphic->mVisibility = visibleState;
                    mpOverlayGraphic->notifyVisible();
                }
                mpOverlayGraphic->draw(fb);
            }
            else
            {
                if (mpOverlayGraphic->mVisibility == visibleState)
                {
                    mpOverlayGraphic->mVisibility = hiddenState;
                    mpOverlayGraphic->notifyHidden();
                }
            }
        }
    }

    void GraphicContext::slideUpTo(Graphic *graphic, int speed)
    {
        if (mpOldGraphic)
        {
            // force finish sliding, release old graphic
            mpOldGraphic->setPosition(mpOldGraphic->mLeft, 0);
            if (mpOldGraphic->mVisibility == visibleState)
            {
                mpOldGraphic->mVisibility = hiddenState;
                mpOldGraphic->notifyHidden();
            }
            mpOldGraphic->release();
            mpOldGraphic = 0;
        }
        mpOldGraphic = mpGraphic;
        mpGraphic = 0;
        setGraphic(graphic);
        mSlidingStep = speed;
        mSlidingOldPosition = 0;
        mSlidingPosition = -64;
    }

    void GraphicContext::slideDownTo(Graphic *graphic, int speed)
    {
        if (mpOldGraphic)
        {
            // force finish sliding, release old graphic
            mpOldGraphic->setPosition(mpOldGraphic->mLeft, 0);
            if (mpOldGraphic->mVisibility == visibleState)
            {
                mpOldGraphic->mVisibility = hiddenState;
                mpOldGraphic->notifyHidden();
            }
            mpOldGraphic->release();
            mpOldGraphic = 0;
        }
        mpOldGraphic = mpGraphic;
        mpGraphic = 0;
        setGraphic(graphic);
        mSlidingStep = -speed;
        mSlidingOldPosition = 0;
        mSlidingPosition = 64;
    }

    void GraphicContext::setGraphic(Graphic *graphic)
    {
        if (mpGraphic)
        {
            if (mpGraphic->mVisibility == visibleState)
            {
                mpGraphic->mVisibility = hiddenState;
                mpGraphic->notifyHidden();
            }
            mpGraphic->release();
        }
        mpGraphic = graphic;
        if (mpGraphic)
        {
            mpGraphic->attach();
        }
    }

    void GraphicContext::setOverlayGraphic(Graphic *graphic)
    {
        if (mpOverlayGraphic)
        {
            if (mpOverlayGraphic->mVisibility == visibleState)
            {
                mpOverlayGraphic->mVisibility = hiddenState;
                mpOverlayGraphic->notifyHidden();
            }
            mpOverlayGraphic->release();
        }
        mpOverlayGraphic = graphic;
        if (mpOverlayGraphic)
        {
            mpOverlayGraphic->attach();
        }
    }

    void GraphicContext::setCursorController(Graphic *graphic)
    {
        if (mpCursorController)
        {
            mpCursorController->mCursorState.active = false;
            mpCursorController->release();
        }
        mpCursorController = graphic;
        if (mpCursorController)
        {
            mpCursorController->attach();
            mpCursorController->mCursorState.active = true;
        }
    }

    void GraphicContext::notifyHidden()
    {
        if (mpOverlayGraphic)
        {
            if (mpOverlayGraphic->mVisibility == visibleState)
            {
                mpOverlayGraphic->mVisibility = hiddenState;
                mpOverlayGraphic->notifyHidden();
            }
        }

        if (mpGraphic)
        {
            if (mpGraphic->mVisibility == visibleState)
            {
                mpGraphic->mVisibility = hiddenState;
                mpGraphic->notifyHidden();
            }
        }
    }

} /* namespace od */
