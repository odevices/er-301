/*
 * RichTextBox.cpp
 *
 *  Created on: 30 Nov 2017
 *      Author: clarkson
 */

#include <od/graphics/text/RichTextBox.h>
#include <od/graphics/fonts.h>
#include <od/extras/Utils.h>

namespace od
{

    RichTextBox::RichTextBox(int left, int bottom, int width, int height) : Graphic(left, bottom, width, height)
    {
    }

    RichTextBox::RichTextBox(const std::string &text, int fontSize) : Graphic(0, 0, 1, 1), mText(text), mFontSize(fontSize)
    {
    }

    RichTextBox::RichTextBox(const std::string &text, int fontSize, int width,
                             int lines) : Graphic(0, 0, 1, 1), mText(text), mFontSize(fontSize)
    {
        const font_family_t &family = *font_info(mFontSize, 0);
        setSize(width, family.line_height * lines + mTopMargin + mBottomMargin);
    }

    RichTextBox::~RichTextBox()
    {
    }

    void RichTextBox::setText(const char *text)
    {
        mText = text;
        mRebuildNeeded = true;
    }

    void RichTextBox::setJustification(Justification justify)
    {
        mJustification = justify;
        mRebuildNeeded = true;
    }

    void RichTextBox::setAlignment(Alignment align)
    {
        mAlignment = align;
        mRebuildNeeded = true;
    }

    void RichTextBox::setMargins(int left, int bottom, int right, int top)
    {
        mLeftMargin = left;
        mBottomMargin = bottom;
        mRightMargin = right;
        mTopMargin = top;
        mRebuildNeeded = true;
    }

    void RichTextBox::setFontSize(int size)
    {
        mFontSize = size;
        mRebuildNeeded = true;
    }

    void RichTextBox::setSize(int width, int height)
    {
        Graphic::setSize(width, height);
        //rebuild();
    }

    void RichTextBox::addLine(const std::string &line, int lineWidth,
                              int targetWidth)
    {
        if (line.size() > 0)
        {
            mLines.push_back(line);
            switch (mJustification)
            {
            case justifyLeft:
                mLineOffsets.push_back(mLeftMargin);
                break;
            case justifyRight:
                mLineOffsets.push_back(targetWidth - mRightMargin - lineWidth);
                break;
            case justifyCenter:
                mLineOffsets.push_back((targetWidth - lineWidth) / 2);
                break;
            }
        }
    }

    bool RichTextBox::flowText(int targetWidth, int maxLines)
    {
        int maxLineWidth = targetWidth - mRightMargin - mLeftMargin;
        int spaceAdvance = getCharacterAdvance(' ', mFontSize);
        bool result = true;

        mLines.clear();
        mLineOffsets.clear();

        std::vector<std::string> lines;
        split(mText, '\n', lines);

        for (std::string &line0 : lines)
        {
            std::vector<std::string> words;
            split(line0, ' ', words);

            bool first = true;
            int x = 0;
            int lineWidth = 0;
            std::string line;

            for (std::string &word : words)
            {
                int wordWidth = getStringWidth(word.c_str(), mFontSize);
                int predictedLineWidth = x + spaceAdvance + wordWidth;
                if (predictedLineWidth < maxLineWidth)
                {
                    // The word fits, so add it the current line.
                    x += getStringAdvance(word.c_str(), mFontSize) + spaceAdvance;
                    if (first)
                    {
                        first = false;
                    }
                    else
                    {
                        line += ' ';
                    }
                    line += word;
                    lineWidth = predictedLineWidth;
                }
                else
                {
                    // The word does not fit, so finish this line and start a new one.
                    addLine(line, lineWidth, targetWidth);

                    if (getLineCount() > maxLines)
                    {
                        // We've reached the bottom of the textbox, so stop here.
                        return false;
                    }

                    if (wordWidth > maxLineWidth)
                    {
                        // This word does not fit in one line.
                        addLine(word, wordWidth, targetWidth);

                        if (getLineCount() > maxLines)
                        {
                            // We've reached the bottom of the textbox, so stop here.
                            return false;
                        }

                        first = true;
                        line.clear();
                        x = 0;
                        result = false;
                    }
                    else
                    {
                        x = getStringAdvance(word.c_str(), mFontSize);
                        line = word;
                        lineWidth = wordWidth;
                    }
                }
            }

            // Handle the last line
            addLine(line, lineWidth, targetWidth);

            if (getLineCount() > maxLines)
            {
                // We've reached the bottom of the textbox, so stop here.
                mRebuildNeeded = false;
                return false;
            }
        }

        mRebuildNeeded = false;
        return result;
    }

    void RichTextBox::fitHeight()
    {
        flowText(mWidth, 9999);
        const font_family_t &family = *font_info(mFontSize, 0);
        int h = mLines.size() * family.line_height;
        Graphic::setSize(mWidth, h + mTopMargin + mBottomMargin);
        mRebuildNeeded = false;
    }

    void RichTextBox::fitHeight(int width)
    {
        flowText(width, 9999);
        const font_family_t &family = *font_info(mFontSize, 0);
        int h = mLines.size() * family.line_height;
        Graphic::setSize(width, h + mTopMargin + mBottomMargin);
        mRebuildNeeded = false;
    }

    void RichTextBox::rebuild()
    {
        const font_family_t &family = *font_info(mFontSize, 0);
        int n = (mHeight - mTopMargin - mBottomMargin) / family.line_height;
        flowText(mWidth, n);
        mRebuildNeeded = false;
    }

    void RichTextBox::draw(FrameBuffer &fb)
    {
        if (mRebuildNeeded)
        {
            rebuild();
        }

        Graphic::draw(fb);

        const font_family_t &family = *font_info(mFontSize, 0);
        int x, y, n = mLines.size();
        if (mAlignment == alignMiddle)
        {
            int top = mWorldBottom + mHeight - mTopMargin - family.base;
            int bottom = y = mWorldBottom + mBottomMargin + n * family.line_height - family.base;
            y = (top + bottom) / 2;
        }
        else if (mAlignment == alignTop)
        {
            y = mWorldBottom + mHeight - mTopMargin - family.base;
        }
        else
        {
            y = mWorldBottom + mBottomMargin + n * family.line_height - family.base;
        }
        for (int i = 0; i < n; i++)
        {
            x = mLineOffsets[i] + mWorldLeft;
            fb.text(mForeground, x, y, mLines[i].data(), mFontSize);
            y -= family.line_height;
        }
    }

} /* namespace od */
