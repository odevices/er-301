#include <od/graphics/text/Label.h>
#include <stdio.h>

namespace od
{

#define DEFAULT_MARGIN 4

    Label::Label(const std::string &text) : Graphic(0, 0, 1, 1)
    {
        setText(text);
        fitToText(DEFAULT_MARGIN);
    }

    Label::Label(const std::string &text, int size) : Graphic(0, 0, 1, 1), mTextSize(size)
    {
        setText(text);
        fitToText(DEFAULT_MARGIN);
    }

    Label::Label(const std::string &text, int size, int direction) : Graphic(0, 0, 1, 1), mTextDirection(direction), mTextSize(size)
    {
        setText(text);
        fitToText(DEFAULT_MARGIN);
    }

    Label::~Label()
    {
        // TODO Auto-generated destructor stub
    }

    void Label::draw(FrameBuffer &fb)
    {
        Graphic::draw(fb);

        if (mTextDirection == TEXT_HORIZONTAL)
        {
            switch (mJustification)
            {
            case justifyLeft:
                fb.text(mForeground, mWorldLeft + mMargin,
                        mWorldBottom + (mHeight - mTextHeight) / 2,
                        mText.c_str(), mTextSize);
                break;
            case justifyCenter:
                fb.text(mForeground, mWorldLeft + (mWidth - mTextWidth) / 2,
                        mWorldBottom + (mHeight - mTextHeight) / 2,
                        mText.c_str(), mTextSize);
                break;
            case justifyRight:
                fb.text(mForeground, mWorldLeft + mWidth - mTextWidth - mMargin,
                        mWorldBottom + (mHeight - mTextHeight) / 2,
                        mText.c_str(), mTextSize);
                break;
            }
        }
        else
        {
            fb.vtext(mForeground, mWorldLeft + (mWidth - mTextWidth) / 2,
                     mWorldBottom + (mHeight - mTextHeight) / 2,
                     mText.c_str(), mTextSize, ALIGN_TOP);
        }
    }

    void Label::setTextDirection(int direction)
    {
        mTextDirection = direction;
        setText(mText);
    }

    void Label::setText(const std::string &text)
    {
        mText = text;
        if (mTextDirection == TEXT_HORIZONTAL)
        {
            getTextSize(mText.c_str(), &mTextWidth, &mTextHeight, mTextSize);
        }
        else
        {
            getTextSize(mText.c_str(), &mTextHeight, &mTextWidth, mTextSize);
        }
    }

    void Label::setTextWithSize(const std::string &text, int size)
    {
        mTextSize = size;
        setText(text);
    }

    void Label::setNumber(float x)
    {
        static char tmp[16];
        snprintf(tmp, sizeof(tmp), "%0.3f", x);
        setText(tmp);
    }

    void Label::fitToText(int margin)
    {
        switch (mJustification)
        {
        case justifyLeft:
            mHeight = mTextHeight + 2 * margin;
            mWidth = mTextWidth + 2 * margin;
            mMargin = margin;
            break;
        case justifyCenter:
        {
            Point center = getCenter();
            mHeight = mTextHeight + 2 * margin;
            mWidth = mTextWidth + 2 * margin;
            setCenter(center.x, center.y);
            mMargin = 0;
        }
        break;
        case justifyRight:
        {
            int right = mLeft + mWidth;
            mHeight = mTextHeight + 2 * margin;
            mWidth = mTextWidth + 2 * margin;
            mMargin = margin;
            setPosition(right - mWidth, mBottom);
        }
        break;
        }
    }

} /* namespace od */
