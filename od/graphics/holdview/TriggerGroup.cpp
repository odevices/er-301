/*
 * TriggerGroup.cpp
 *
 *  Created on: 29 May 2019
 *      Author: clarkson
 */

#include <od/graphics/holdview/TriggerGroup.h>
#include <algorithm>

namespace od
{

    TriggerGroup::TriggerGroup() : Graphic(0, 0, SECTION_PLY, SCREEN_HEIGHT)
    {
        own(mNameLabel);
        addChild(&mNameLabel);
        mNameLabel.setCenter(mWidth / 2);
        mCursorState.orientation = CursorOrientation::cursorDown;
    }

    TriggerGroup::~TriggerGroup()
    {
        clear();
    }

    void TriggerGroup::draw(FrameBuffer &fb)
    {
        Graphic::draw(fb);

        int L = mWidth / 2;
        int hL = L / 2;
        int left = mWorldLeft + L - hL;
        int bottom = mWorldBottom + GRID4_LINE2 - hL;
        int right = left + L;
        int top = bottom + L + 1;

        int i = 0;
        for (; i < mFill; i += 2)
        {
            fb.hline(WHITE, left, right, bottom + i);
        }
        for (; i < L; i += 2)
        {
            fb.hline(GRAY3, left, right, bottom + i);
        }

        if (mPressed)
        {
            fb.box(WHITE, left, bottom, right, top);
        }
        else
        {
            fb.box(GRAY7, left, bottom, right, top);
        }

        if (mPressed)
        {
            if (mFill < hL)
            {
                mFill = hL;
            }
            else
            {
                mFill = L;
            }
        }
        else
        {
            if (mFill > hL)
            {
                mFill = hL;
            }
            else
            {
                mFill = 0;
            }
        }
    }

    void TriggerGroup::setLabel(const std::string &text)
    {
        mNameLabel.setText(text);
        mNameLabel.setCenter(mWidth / 2);
    }

    const std::string &TriggerGroup::getLabel()
    {
        return mNameLabel.getText();
    }

    void TriggerGroup::setLabelCenter(int x)
    {
        mNameLabel.setCenter(x);
    }

    void TriggerGroup::showLabel()
    {
        mNameLabel.show();
    }

    void TriggerGroup::hideLabel()
    {
        mNameLabel.hide();
    }

    void TriggerGroup::onPressed()
    {
        for (Comparator *o : mComparators)
        {
            o->simulateRisingEdge();
        }
        mPressed = true;
    }

    void TriggerGroup::onReleased()
    {
        for (Comparator *o : mComparators)
        {
            o->simulateFallingEdge();
        }
        mPressed = false;
    }

    void TriggerGroup::add(Comparator *o)
    {
        if (o)
        {
            auto i = std::find(mComparators.begin(), mComparators.end(), o);
            if (i == mComparators.end())
            {
                o->attach();
                mComparators.push_back(o);
            }
        }
    }

    void TriggerGroup::remove(Comparator *o)
    {
        auto i = std::find(mComparators.begin(), mComparators.end(), o);
        if (i != mComparators.end())
        {
            o->release();
            mComparators.erase(i);
        }
    }

    void TriggerGroup::clear()
    {
        for (Comparator *o : mComparators)
        {
            o->release();
        }
        mComparators.clear();
    }

    int TriggerGroup::size()
    {
        return mComparators.size();
    }

} /* namespace od */
