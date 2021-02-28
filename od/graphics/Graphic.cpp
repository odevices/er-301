/*
 * Graphic.cpp
 *
 *  Created on: 7 May 2016
 *      Author: clarkson
 */

#include <od/graphics/Graphic.h>
#include <hal/constants.h>
#include <algorithm>

namespace od
{
#define TWEENSTEP (2.0f / GRAPHICS_REFRESH_RATE)
    float Graphic::sTween = 0.0f;
    float Graphic::sTweenStep = TWEENSTEP;

    Graphic NullGraphic(0, 0, 10000, 10000);

    void Graphic::updateTween()
    {
        sTween += sTweenStep;
        if (sTween > 1.0f)
        {
            sTween = 1.0f;
            sTweenStep = -sTweenStep;
        }
        else if (sTween < 0.0f)
        {
            sTween = 0.0f;
            sTweenStep = -sTweenStep;
        }
    }

    void Graphic::resetTween()
    {
        sTween = 0.0f;
        sTweenStep = TWEENSTEP;
    }

    Graphic::Graphic(int left, int bottom, int width, int height) : mLeft(left), mBottom(bottom), mWidth(width), mHeight(height), mpParent(
                                                                                                                                      &NullGraphic)
    {
        updateWorldCoordinates();
    }

    Graphic::~Graphic()
    {
        clear();
    }

    void Graphic::clear()
    {
        for (Graphic *graphic : mChildren)
        {
            if (graphic->mVisibility == visibleState)
            {
                graphic->mVisibility = hiddenState;
                graphic->notifyHidden();
            }
            graphic->mpParent = &NullGraphic;
            graphic->updateWorldCoordinates();
            if (!owned(graphic))
            {
                graphic->release();
            }
        }
        mChildren.clear();
    }

    bool Graphic::contains(Graphic *target)
    {
        for (Graphic *graphic : mChildren)
        {
            if (graphic == target)
            {
                return true;
            }
        }
        return false;
    }

    // Does not need to be called if no border and no children.
    void Graphic::draw(FrameBuffer &fb)
    {
        if (mOpaque)
        {
            if (mBackground > BLACK)
            {
                fb.fill(mBackground, mWorldLeft, mWorldBottom,
                        mWorldLeft + mWidth - 1, mWorldBottom + mHeight - 1);
            }
            else
            {
                fb.clear(mWorldLeft, mWorldBottom, mWorldLeft + mWidth,
                         mWorldBottom + mHeight);
            }
        }

        if (mBorder > 0)
        {
            for (int i = 0; i < mBorder; i++)
            {
                int left = mWorldLeft + i;
                int bottom = mWorldBottom + i;
                int right = mWorldLeft + mWidth - 1 - i;
                int top = mWorldBottom + mHeight - 1 - i;
                // top border
                fb.hline(mBorderColor, left + mBorderRadiusTL,
                         right - mBorderRadiusTR, top);
                // bottom border
                fb.hline(mBorderColor, left + mBorderRadiusBL,
                         right - mBorderRadiusBR, bottom);
                // left border
                fb.vline(mBorderColor, left, bottom + mBorderRadiusBL,
                         top - mBorderRadiusTL);
                // right border
                fb.vline(mBorderColor, right, bottom + mBorderRadiusBR,
                         top - mBorderRadiusTR);

                // top-left corner
                fb.arc8(mBorderColor, left + mBorderRadiusTL, top - mBorderRadiusTL,
                        mBorderRadiusTL, 0b00001100);
                // top-right corner
                fb.arc8(mBorderColor, right - mBorderRadiusTR,
                        top - mBorderRadiusTR, mBorderRadiusTR, 0b00000011);
                // bottom-right corner
                fb.arc8(mBorderColor, right - mBorderRadiusBR,
                        bottom + mBorderRadiusBR, mBorderRadiusBR, 0b11000000);
                // bottom-left corner
                fb.arc8(mBorderColor, left + mBorderRadiusBL,
                        bottom + mBorderRadiusBL, mBorderRadiusBL, 0b00110000);
            }
        }

        for (Graphic *graphic : mChildren)
        {
            if (graphic->visible(fb))
            {
                if (graphic->mVisibility == hiddenState)
                {
                    graphic->mVisibility = visibleState;
                    graphic->notifyVisible();
                }
                graphic->draw(fb);
            }
            else
            {
                if (graphic->mVisibility == visibleState)
                {
                    graphic->mVisibility = hiddenState;
                    graphic->notifyHidden();
                }
            }
        }
    }

    void Graphic::addChild(Graphic *graphic)
    {
        if (graphic == this || graphic->mpParent != &NullGraphic)
        {
            return;
        }
        if (!owned(graphic))
        {
            graphic->attach();
        }
        mChildren.push_back(graphic);
        graphic->mpParent = this;
        graphic->updateWorldCoordinates();
    }

    void Graphic::addChildOnce(Graphic *graphic)
    {
        if (!contains(graphic))
        {
            addChild(graphic);
        }
    }

    void Graphic::removeChild(Graphic *graphic)
    {
        std::vector<Graphic *>::iterator i = std::find(mChildren.begin(),
                                                       mChildren.end(), graphic);
        if (i != mChildren.end())
        {
            if (graphic->mVisibility == visibleState)
            {
                graphic->mVisibility = hiddenState;
                graphic->notifyHidden();
            }
            graphic->mpParent = &NullGraphic;
            graphic->updateWorldCoordinates();
            if (!owned(graphic))
            {
                graphic->release();
            }
            mChildren.erase(i);
        }
    }

    void Graphic::setCenter(int x, int y)
    {
        Point center = getCenter();
        setPosition(mLeft + x - center.x, mBottom + y - center.y);
    }

    Rect Graphic::getBoundingRect()
    {
        Rect boundingRect;

        boundingRect = getWorldRect();

        for (Graphic *graphic : mChildren)
        {
            boundingRect = merge(boundingRect, graphic->getBoundingRect());
        }

        return boundingRect;
    }

    void Graphic::notifyVisible()
    {
    }

    void Graphic::notifyHidden()
    {
        for (Graphic *graphic : mChildren)
        {
            if (graphic->mVisibility == visibleState)
            {
                graphic->mVisibility = hiddenState;
                graphic->notifyHidden();
            }
        }
    }

    void Graphic::notifyContentsChanged()
    {
    }

    void Graphic::saveSize()
    {
        mSavedWidth = mWidth;
        mSavedHeight = mHeight;
        mSizeSaved = true;
    }

    void Graphic::restoreSize()
    {
        if (mSizeSaved)
        {
            setSize(mSavedWidth, mSavedHeight, false);
        }
    }

    void Graphic::expandToPly(int padding)
    {
        restoreSize();
        int w = ((mWidth + padding) / SECTION_PLY + 1) * SECTION_PLY;
        setSize(w - padding, mHeight, false);
    }

    bool Graphic::intersectsWithButton(int button)
    {
        int centers[] = {BUTTON1_CENTER,
                         BUTTON2_CENTER,
                         BUTTON3_CENTER,
                         BUTTON4_CENTER,
                         BUTTON5_CENTER,
                         BUTTON6_CENTER};
        Rect target;
        target.bottom = mWorldBottom;
        target.top = mWorldBottom + mHeight;

        if (button < 0)
        {
            target.left = centers[0] - 1;
        }
        if (button > 5)
        {
            target.left = centers[5] - 1;
        }
        else
        {
            target.left = centers[button] - 1;
        }
        target.right = target.left + 2;

        return intersects(target);
    }

    void Graphic::setForegroundColor(Color color)
    {
        mForeground = color;
    }

    void Graphic::setBackgroundColor(Color color)
    {
        mBackground = color;
    }

    void Graphic::setBorderColor(Color color)
    {
        mBorderColor = color;
    }

    void Graphic::setBorder(int thickness)
    {
        mBorder = thickness;
    }

    void Graphic::setCornerRadius(int bl, int tl, int tr, int br)
    {
        mBorderRadiusBL = bl;
        mBorderRadiusTL = tl;
        mBorderRadiusTR = tr;
        mBorderRadiusBR = br;
    }

    void Graphic::setOpaque(bool value)
    {
        mOpaque = value;
    }

    void Graphic::setPosition(int left, int bottom)
    {
        mLeft = left;
        mBottom = bottom;
        updateWorldCoordinates();
    }

    void Graphic::setSize(int width, int height, bool updateSaved)
    {
        mWidth = width;
        mHeight = height;
        if (updateSaved)
        {
            saveSize();
        }
    }

    void Graphic::hide()
    {
        mHidden = true;
    }

    void Graphic::show()
    {
        mHidden = false;
    }

    bool Graphic::isHidden()
    {
        return mHidden;
    }

    bool Graphic::hasParent()
    {
        return mpParent != &NullGraphic;
    }

    void Graphic::updateWorldCoordinates()
    {
        mWorldLeft = mLeft + mpParent->mWorldLeft;
        mWorldBottom = mBottom + mpParent->mWorldBottom;
        // propagate
        for (Graphic *graphic : mChildren)
        {
            graphic->updateWorldCoordinates();
        }
    }

    Point Graphic::getCenter()
    {
        Point pt;
        pt.x = mLeft + mWidth / 2;
        pt.y = mBottom + mHeight / 2;
        return pt;
    }

    Size Graphic::getSize()
    {
        Size sz;
        sz.width = mWidth;
        sz.height = mHeight;
        return sz;
    }

    Rect Graphic::getRect()
    {
        Rect rect;
        rect.left = mLeft;
        rect.bottom = mBottom;
        rect.right = mLeft + mWidth;
        rect.top = mBottom + mHeight;
        return rect;
    }

    Rect Graphic::getWorldRect()
    {
        Rect rect;
        rect.left = mWorldLeft;
        rect.bottom = mWorldBottom;
        rect.right = mWorldLeft + mWidth;
        rect.top = mWorldBottom + mHeight;
        return rect;
    }

    bool Graphic::intersects(Graphic &w)
    {
        return !(w.mWorldLeft > mWorldLeft + mWidth || w.mWorldLeft + w.mWidth < mWorldLeft || w.mWorldBottom + w.mHeight < mWorldBottom || w.mWorldBottom > mWorldBottom + mHeight);
    }

    bool Graphic::intersects(Rect &r)
    {
        return !(r.left > mWorldLeft + mWidth || r.right < mWorldLeft || r.top < mWorldBottom || r.bottom > mWorldBottom + mHeight);
    }

    bool Graphic::visible(FrameBuffer &fb)
    {
        return !(mHidden || 0 > mWorldLeft + mWidth || fb.mWidth < mWorldLeft || fb.mHeight < mWorldBottom || 0 > mWorldBottom + mHeight);
    }

    bool Graphic::notVisible(FrameBuffer &fb)
    {
        return (0 > mWorldLeft + mWidth || fb.mWidth < mWorldLeft || fb.mHeight < mWorldBottom || 0 > mWorldBottom + mHeight);
    }

} /* namespace od */
