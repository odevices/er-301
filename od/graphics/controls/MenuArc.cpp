
#include <od/graphics/controls/MenuArc.h>
#include <od/graphics/fonts.h>
#include <math.h>

#define SELSIZE 10

namespace od
{

    const float arcOffsets[] = {0, sin(M_PI * 0.167), sin(M_PI * 0.333), 1, sin(M_PI * 0.333),
                                sin(M_PI * 0.167), 0};

    static inline float interpolateArc(int i, float w)
    {
        i += 3;
        if (i < 0)
        {
            return 0;
        }
        if (i > 5)
        {
            return 0;
        }
        return (1 - w) * arcOffsets[i] + w * arcOffsets[i + 1];
    }

    MenuArc::Item::Item(const std::string &text) : text(text), x(0), y(0)
    {
        width = getStringWidth(text.c_str(), 10);
        selectedWidth = getStringWidth(text.c_str(), SELSIZE);
    }

    MenuArc::MenuArc() : Graphic(0, 0, 256, 64)
    {
    }

    MenuArc::~MenuArc()
    {
    }

    void MenuArc::Item::draw(FrameBuffer &fb, int left, int mid,
                             bool leftJustified, bool selected)
    {
        mid += y;

        if (leftJustified)
        {
            left += x;
        }
        else
        {
            left -= x + selected ? selectedWidth : width;
        }

        if (selected)
        {
            fb.clear(left - 2, mid - 6, x + selectedWidth + 2, mid + 6);
            fb.drawBeveledBox(GRAY11, left - 2, mid - 6, left + selectedWidth + 2,
                              mid + 6);
            fb.text(WHITE, left, mid, text.c_str(), SELSIZE, ALIGN_MIDDLE);
        }
        else
        {
            fb.clear(left - 2, mid - 7, left + width + 2, mid + 5);
            fb.drawBeveledBox(GRAY11, left - 2, mid - 7, left + width + 2,
                              mid + 5);
            fb.text(GRAY11, left, mid, text.c_str(), 10, ALIGN_MIDDLE);
        }
    }

    void MenuArc::draw(FrameBuffer &fb)
    {
        if (mContents.size() == 0)
        {
            return;
        }

        mRenderIndex = 0.8f * mRenderIndex + 0.2f * mSelectedIndex;
        if (fabs(mRenderIndex - mSelectedIndex) < 0.02f)
        {
            mRenderIndex = mSelectedIndex;
        }

        int renderInteger = mRenderIndex;
        float renderFraction = mRenderIndex - renderInteger;
        int mid = mWorldBottom + mHeight / 2;
        int left = mWorldLeft + mAnchor;
        int n = size();

        if (mBuiltIndex != mRenderIndex)
        {
            mBuiltIndex = mRenderIndex;
            for (int i = 0; i < n; i++)
            {
                Item &item = mContents[i];
                int d = i - renderInteger;
                item.x = cRadius * interpolateArc(d - 1, 1.0f - renderFraction);
                item.y = (renderFraction - d) * 10;
            }
        }

        //fb.circle(GRAY11, left, mid, 15);
        if (mLeftJustified)
        {
            for (int i = 0; i < mSelectedIndex; i++)
            {
                Item &item = mContents[i];
                int y = item.y + mid;
                if (y > mWorldBottom && y < mWorldBottom + mHeight)
                {
                    int x = item.x + left;
                    fb.clear(x - 2, y - 7, x + item.width + 2, y + 5);
                    fb.drawBeveledBox(GRAY11, x - 2, y - 7, x + item.width + 2,
                                      y + 5);
                    fb.text(GRAY11, x, y, item.text.c_str(), 10, ALIGN_MIDDLE);
                }
            }

            for (int i = n - 1; i > mSelectedIndex; i--)
            {
                Item &item = mContents[i];
                int y = item.y + mid;
                if (y > mWorldBottom && y < mWorldBottom + mHeight)
                {
                    int x = item.x + left;
                    fb.clear(x - 2, y - 7, x + item.width + 2, y + 5);
                    fb.drawBeveledBox(GRAY11, x - 2, y - 7, x + item.width + 2,
                                      y + 5);
                    fb.text(GRAY11, x, y, item.text.c_str(), 10, ALIGN_MIDDLE);
                }
            }

            // draw the selected item last
            {
                Item &item = mContents[mSelectedIndex];
                int y = item.y + mid;
                int x = item.x + left;
                fb.clear(x - 3, y - 7, x + item.selectedWidth + 3, y + 7);
                fb.drawBeveledBox(GRAY11, x - 3, y - 8, x + item.selectedWidth + 3,
                                  y + 8);
                fb.text(mForeground, x, y, item.text.c_str(), SELSIZE,
                        ALIGN_MIDDLE);

                mCursorState.orientation = cursorRight;
                mCursorState.x = x;
                mCursorState.y = y;
            }
        }
        else
        {
            for (int i = 0; i < n; i++)
            {
                Item &item = mContents[i];
                int y = item.y + mid;
                if (y > mWorldBottom && y < mWorldBottom + mHeight)
                {
                    if (i != mSelectedIndex)
                    {
                        int x = left - item.x - item.width;
                        fb.clear(x - 2, y - 6, x + item.width + 2, y + 6);
                        fb.drawBeveledBox(GRAY11, x - 2, y - 6, x + item.width + 2,
                                          y + 6);
                        fb.text(mForeground, x, y, item.text.c_str(), 10,
                                ALIGN_MIDDLE);
                    }
                }
            }

            // draw the selected item last
            {
                Item &item = mContents[mSelectedIndex];
                int y = item.y + mid;
                int x = left - item.x - item.selectedWidth;
                fb.clear(x - 2, y - 6, x + item.selectedWidth + 2, y + 6);
                fb.drawBeveledBox(GRAY11, x - 2, y - 6, x + item.selectedWidth + 2,
                                  y + 6);
                fb.text(mForeground, x, y, item.text.c_str(), SELSIZE,
                        ALIGN_MIDDLE);

                mCursorState.orientation = cursorLeft;
                mCursorState.x = x + item.selectedWidth;
                mCursorState.y = y;
            }
        }
    }

    void MenuArc::setAnchor(int x)
    {
        mAnchor = x;
        mLeftJustified = mAnchor + mMaxTextWidth + cRadius < mWidth;
    }

    void MenuArc::add(const std::string &text)
    {
        mContents.emplace_back(text);
        mSelectedIndex = 0;
        mBuiltIndex = -1;
        Item &item = mContents.back();
        if (item.selectedWidth > mMaxTextWidth)
        {
            mMaxTextWidth = item.selectedWidth;
            mLeftJustified = mAnchor + mMaxTextWidth + cRadius < mWidth;
        }
    }

    void MenuArc::clear()
    {
        mContents.clear();
        mSelectedIndex = 0;
        mBuiltIndex = -1;
        mMaxTextWidth = 0;
        mLeftJustified = true;
    }

    int MenuArc::size()
    {
        return (int)mContents.size();
    }

    bool MenuArc::select(const std::string &text)
    {
        int n = size();
        for (int i = 0; i < n; i++)
        {
            if (text == mContents.at(i).text)
            {
                mSelectedIndex = i;
                return true;
            }
        }
        return false;
    }

    bool MenuArc::select(int index)
    {
        if (index >= 0 && index < size())
        {
            mSelectedIndex = index;
            return true;
        }
        return false;
    }

    const std::string &MenuArc::selectedText()
    {
        static std::string empty{""};
        if (mContents.size() == 0)
        {
            return empty;
        }

        return mContents.at(mSelectedIndex).text;
    }

    int MenuArc::selectedIndex()
    {
        return mSelectedIndex;
    }

    void MenuArc::scrollUp()
    {
        if (mContents.size() == 0)
            return;
        mSelectedIndex = MAX(0, mSelectedIndex - 1);
    }

    void MenuArc::scrollDown()
    {
        if (mContents.size() == 0)
            return;
        mSelectedIndex = MIN(size() - 1, mSelectedIndex + 1);
    }

    void MenuArc::scrollToBottom()
    {
        if (mContents.size() == 0)
            return;
        mSelectedIndex = size() - 1;
    }

    void MenuArc::scrollToTop()
    {
        if (mContents.size() == 0)
            return;
        mSelectedIndex = 0;
    }

    bool MenuArc::encoder(int change, bool shifted, int threshold)
    {
        if (mLeftJustified)
        {
            mEncoderSum += change;
        }
        else
        {
            mEncoderSum -= change;
        }

        if (mEncoderSum > threshold)
        {
            mEncoderSum = 0;
            if (shifted)
            {
                scrollToTop();
            }
            else
            {
                scrollUp();
            }
            return true;
        }
        else if (mEncoderSum < -threshold)
        {
            mEncoderSum = 0;
            if (shifted)
            {
                scrollToBottom();
            }
            else
            {
                scrollDown();
            }
            return true;
        }

        return false;
    }

} /* namespace od */
