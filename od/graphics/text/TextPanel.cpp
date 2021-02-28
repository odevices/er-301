/*
 * Panel.cpp
 *
 *  Created on: 8 Apr 2017
 *      Author: clarkson
 */

#include <od/graphics/text/TextPanel.h>
#include <od/extras/Utils.h>
#include <od/graphics/fonts.h>

namespace od
{

    TextPanel::TextPanel(const std::string &text, int column, float textRatio,
                         int height) : Graphic(0, 0, SECTION_PLY, height)
    {
        setCenter((column - 1) * 43 + 20, height / 2);
        if (column > 1)
        {
            mLeftBorder = 1;
        }
        mBorderColor = GRAY3;
        mTextHeight = height * textRatio;
        setText(text);
        mCursorState.orientation = cursorRight;
    }

    TextPanel::~TextPanel()
    {
    }

    void TextPanel::clear()
    {
        mLines.clear();
    }

    void TextPanel::setText(const std::string &text)
    {
        mLines.clear();
        split(text, ' ', mLines);
    }

    void TextPanel::addLine(const std::string &line)
    {
        mLines.emplace_back(line);
    }

    void TextPanel::setLine(int index, const std::string &line)
    {
        if (index == getLineCount())
        {
            mLines.emplace_back(line);
        }
        else if (index >= 0 && index < getLineCount())
        {
            mLines[index] = line;
        }
    }

#define CORNER_RADIUS 5
    void TextPanel::imitateSectionStyle(SectionStyle style)
    {
        switch (style)
        {
        case sectionNoBorder:
            setBorder(0);
            setCornerRadius(0, 0, 0, 0);
            break;
        case sectionSimple:
            setBorder(1);
            setCornerRadius(0, 0, 0, 0);
            mBorderColor = WHITE;
            break;
        case sectionBegin:
            setBorder(1);
            setCornerRadius(2 * CORNER_RADIUS, 2 * CORNER_RADIUS, 0, 0);
            mBorderColor = GRAY11;
            break;
        case sectionMiddle:
            setBorder(1);
            setCornerRadius(CORNER_RADIUS, CORNER_RADIUS, CORNER_RADIUS,
                            CORNER_RADIUS);
            mBorderColor = GRAY7;
            break;
        case sectionEnd:
            setBorder(1);
            setCornerRadius(0, 0, 2 * CORNER_RADIUS, 2 * CORNER_RADIUS);
            mBorderColor = GRAY11;
            break;
        }
        mLeftBorder = 0;
        mRightBorder = 0;
    }

    void TextPanel::draw(FrameBuffer &fb)
    {
        Graphic::draw(fb);

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

        int n = getLineCount();
        const font_family_t &family = *font_info(mTextSize, 0);
        int y = mWorldBottom + mTextHeight + (n - 1) * family.line_height / 2;
        for (std::string &line : mLines)
        {
            switch (mJustification)
            {
            case justifyLeft:
                fb.text(mForeground, mWorldLeft + mMargin, y, line.c_str(),
                        mTextSize, ALIGN_MIDDLE);
                break;
            case justifyCenter:
            {
                int textWidth, textHeight;
                getTextSize(line.c_str(), &textWidth, &textHeight, mTextSize);
                fb.text(mForeground, mWorldLeft + (mWidth - textWidth) / 2, y,
                        line.c_str(), mTextSize, ALIGN_MIDDLE);
            }
            break;
            case justifyRight:
            {
                int textWidth, textHeight;
                getTextSize(line.c_str(), &textWidth, &textHeight, mTextSize);
                fb.text(mForeground, mWorldLeft + mWidth - textWidth - mMargin - 2,
                        y, line.c_str(), mTextSize, ALIGN_MIDDLE);
            }
            break;
            }
            y -= family.line_height;
        }

        if (mSelected)
        {
            fb.box(WHITE, mWorldLeft + 1, mWorldBottom + 1, mWorldLeft + mWidth - 1,
                   mWorldBottom + mHeight - 2);
        }

        mCursorState.x = mWorldLeft;
        mCursorState.y = mWorldBottom + mHeight / 2;
    }

} /* namespace od */
