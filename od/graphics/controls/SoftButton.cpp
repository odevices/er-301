/*
 * SoftButton.cpp
 *
 *  Created on: 24 Mar 2017
 *      Author: clarkson
 */

#include <od/graphics/controls/SoftButton.h>
#include <stdio.h>

#define MARGIN 4
#define TEXTSIZE 10
#define CORNER 0
#define YCENTER 6

namespace od
{

    SoftButton::SoftButton(const std::string &text, int column) : Graphic(0, 0, SECTION_PLY, TEXTSIZE + 3)
    {
        setCenter((column - 1) * 43 + 20);
        setText(text);
        mBorderColor = GRAY3;
        if (column > 1)
        {
            mLeftBorder = 1;
        }
    }

    SoftButton::~SoftButton()
    {
    }

    void SoftButton::setText(const std::string &text)
    {
        mText = text;
        mDisplayText = text;
        shortenText(mDisplayText, mWidth - MARGIN, TEXTSIZE);
        getTextSize(mDisplayText.c_str(), &mTextWidth, &mTextHeight, TEXTSIZE);
        Point center = getCenter();
        setCenter(center.x);
    }

    void SoftButton::setNumber(float x)
    {
        static char tmp[16];
        snprintf(tmp, sizeof(tmp), "%0.3f", x);
        setText(tmp);
    }

    void SoftButton::setCenter(int x)
    {
        Graphic::setCenter(x, YCENTER);
    }

    void SoftButton::draw(FrameBuffer &fb)
    {
        int textLeft = mWorldLeft + (mWidth - mTextWidth) / 2;
        int textBottom = mWorldBottom + (mHeight - mTextHeight) / 2;
        if (mOpaque)
        {
            // buttons are opaque
            fb.clear(textLeft, mWorldBottom + 1, textLeft + mTextWidth,
                     textBottom + mTextHeight - 1);
        }
        // justify center
        fb.text(mForeground, textLeft, textBottom, mDisplayText.c_str(), TEXTSIZE);

        if (mInvert)
        {
            fb.invert(textLeft, mWorldBottom, textLeft + mTextWidth,
                      textBottom + mTextHeight);
        }

        if (mTopBorder)
        {
            // top border
            fb.hline(mBorderColor, mWorldLeft, mWorldLeft + mWidth,
                     mWorldBottom + mHeight - 1);
        }

        if (mLeftBorder)
        {
            fb.vline(mBorderColor, mWorldLeft, mWorldBottom,
                     mWorldBottom + mHeight);
        }

        if (mRightBorder)
        {
            fb.vline(mBorderColor, mWorldLeft + mWidth - 1, mWorldBottom,
                     mWorldBottom + mHeight);
        }
    }

} /* namespace od */
