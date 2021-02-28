/*
 * SliceList.cpp
 *
 *  Created on: 22 Oct 2016
 *      Author: clarkson
 */

#include <od/graphics/sampling/SliceList.h>
#include <stdio.h>

extern "C"
{
#include <od/graphics/fonts.h>
}

namespace od
{

    SliceList::SliceList(int left, int bottom, int width, int height) : Graphic(left, bottom, width, height)
    {
        mHeightInLines = mHeight / mTextSize;
        mCursorState.orientation = cursorRight;
    }

    SliceList::~SliceList()
    {
        if (mpSlices)
        {
            mpSlices->release();
            mpSlices = 0;
        }
    }

    void SliceList::setSlices(Slices *slices)
    {
        if (mpSlices != slices)
        {
            if (mpSlices)
            {
                mpSlices->release();
            }
            mpSlices = slices;
            if (mpSlices)
            {
                mpSlices->attach();
            }
            mSelectIndex = mTopIndex = 0;
            sync();
        }
    }

    static void timeString(int i, float totalSecs, char *result, int n)
    {
        int hours = totalSecs / 3600;
        totalSecs -= hours * 3600;
        int mins = totalSecs / 60;
        totalSecs -= mins * 60;
        int secs = totalSecs;
        totalSecs -= secs;
        int ms = 1000 * totalSecs;
        snprintf(result, n, "%03d - %02d:%02d.%03d", i, mins, secs, ms);
    }

    void SliceList::draw(FrameBuffer &fb)
    {
        Graphic::draw(fb);

        int y;
        int left, bottom, right, top, textLeft;
        left = mWorldLeft;
        bottom = mWorldBottom;
        right = left + mWidth;
        top = bottom + mHeight;

        textLeft = left + mLeftMargin;

        if (fb.mIsMonoChrome)
        {
            // make room for selection triangle
            textLeft += 3;
        }

        if (mpSlices == 0 || mpSlices->size() == 0)
        {
            fb.text(mForeground, textLeft, bottom + mHeight / 2 + mTextSize / 2,
                    "No slices.");
            return;
        }

        sync();

        // draw selection
        if (mShowSelection)
        {
            int y0, y1;
            y0 = top - (mSelectIndex - mTopIndex + 1) * mTextSize;
            y1 = y0 + mTextSize - 1;
            mCursorState.x = left;
            mCursorState.y = (y0 + y1) / 2;
            if (mHasFocus)
            {
                if (fb.mIsMonoChrome)
                {
                    fb.drawRightTriangle(WHITE, left, (y0 + y1) / 2, 5);
                }
                else
                {
                    fb.fill(GRAY3, left + 2, y0, right - 4, y1);
                }
            }
            else
            {
                if (fb.mIsMonoChrome)
                {
                    fb.box(WHITE, left, y0, left + 3, y1);
                }
                else
                {
                    fb.box(GRAY3, left + 2, y0, right - 4, y1);
                }
            }
        }

        // render from the top and progress down until we leave the bounding box
        int i, j, n = (int)mpSlices->size();
        char tmp[32];
        for (i = mTopIndex, j = 0, y = top - mTextSize / 2; i < n && y >= bottom;
             i++, j++, y -= mTextSize)
        {
            float secs = mpSlices->getPositionInSeconds(i);
            timeString(i, secs, tmp, sizeof(tmp));
            fb.text(mForeground, textLeft, y, tmp, mTextSize, ALIGN_MIDDLE);
        }

        // draw scroll bar
        float scrollSize, scrollPos;
        //scrollSize = (j - 1) / (float) mContents.size();
        scrollSize = j / (float)n;
        scrollPos = mTopIndex / (float)n;
        int scroll0, scroll1;
        scroll0 = top - scrollPos * mHeight;
        scroll1 = scroll0 - scrollSize * mHeight;
        fb.line(mForeground, right - 2, scroll0, right - 2, scroll1);
    }

    void SliceList::scrollUp()
    {
        if (mpSlices == 0 || mpSlices->size() == 0)
            return;

        if (mSelectIndex > 0)
        {
            mSelectIndex--;
            if ((mSelectIndex - mTopIndex < mHeightInLines / 2) && (mTopIndex > 0))
            {
                mTopIndex--;
            }
        }
    }

    void SliceList::scrollDown()
    {
        if (mpSlices == 0 || mpSlices->size() == 0)
            return;

        if (mSelectIndex + 1 < (int)mpSlices->size())
        {
            mSelectIndex++;
            if ((mSelectIndex - mTopIndex >= mHeightInLines / 2) && (mTopIndex + mHeightInLines < (int)mpSlices->size()))
            {
                mTopIndex++;
            }
        }
    }

    void SliceList::scrollToBottom()
    {
        if (mpSlices == 0 || mpSlices->size() == 0)
            return;

        while (mSelectIndex + 1 < (int)mpSlices->size())
        {
            scrollDown();
        }
    }

    void SliceList::scrollToTop()
    {
        if (mpSlices == 0 || mpSlices->size() == 0)
            return;

        mSelectIndex = mTopIndex = 0;
    }

    // Make sure the selection is still valid in the case of changes in the Slices data.
    void SliceList::sync()
    {
        if (mpSlices)
        {
            if (mSelectIndex >= (int)(mpSlices->size()))
            {
                mSelectIndex = mTopIndex = 0;
                scrollToBottom();
            }
        }
    }

    bool SliceList::encoder(int change, bool shifted, int threshold)
    {
        mEncoderSum += change;
        if (mEncoderSum > threshold)
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
        else if (mEncoderSum < -threshold)
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
        return false;
    }

    uint32_t SliceList::getSelectedPosition()
    {
        if (mpSlices)
        {
            Slice *slice = mpSlices->get(mSelectIndex);
            if (slice)
            {
                return slice->mStart;
            }
        }
        return 0;
    }

    void SliceList::selectNearest(uint32_t position)
    {
        if (mpSlices)
        {
            SlicesIterator i = mpSlices->findNearest(position);
            if (i != mpSlices->end())
            {
                mSelectIndex = (int)(i - mpSlices->begin());
                if (mpSlices->size() < mHeightInLines)
                {
                    mTopIndex = 0;
                }
                else
                {
                    mTopIndex = MIN(mSelectIndex,
                                    mpSlices->size() - mHeightInLines);
                }
            }
        }
    }

} /* namespace od */
