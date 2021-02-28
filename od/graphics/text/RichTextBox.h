#pragma once

#include <od/graphics/Graphic.h>
#include <string>
#include <vector>

namespace od
{

    class RichTextBox : public Graphic
    {
    public:
        RichTextBox(int left, int bottom, int width, int height);
        RichTextBox(const std::string &text, int fontSize, int width, int lines);
        RichTextBox(const std::string &text, int fontSize);
        virtual ~RichTextBox();

        void setText(const char *text);
        const std::string &getText()
        {
            return mText;
        }
        void setJustification(Justification justify);
        void setAlignment(Alignment align);
        void setMargins(int left, int bottom, int right, int top);
        void setFontSize(int size);
        virtual void setSize(int width, int height);
        void fitHeight(int width);
        void fitHeight();

        // returns false if it could not fit the text
        bool flowText(int targetWidth, int maxLines);
        void rebuild();

        int getLineCount()
        {
            return mLines.size();
        }

#ifndef SWIGLUA
        virtual void draw(FrameBuffer &fb);
#endif

    protected:
        std::string mText;
        std::vector<std::string> mLines;
        std::vector<int> mLineOffsets;

        int mTopMargin = 1;
        int mBottomMargin = 1;
        int mLeftMargin = 1;
        int mRightMargin = 1;
        Justification mJustification = justifyLeft;
        Alignment mAlignment = alignMiddle;
        int mFontSize = 10;
        bool mRebuildNeeded = false;

        void addLine(const std::string &line, int lineWidth, int targetWidth);
    };

} /* namespace od */
