/*
 * SliceList.h
 *
 *  Created on: 22 Oct 2016
 *      Author: clarkson
 */

#ifndef APP_GRAPHICS_SLICELIST_H_
#define APP_GRAPHICS_SLICELIST_H_

#include <od/graphics/Graphic.h>
#include <od/audio/Slices.h>
#include <string>

namespace od
{

    class SliceList : public Graphic
    {
    public:
        SliceList(int left, int bottom, int width, int height);
        virtual ~SliceList();

#ifndef SWIGLUA
        virtual void draw(FrameBuffer &fb);
#endif

        void setSlices(Slices *slices);
        void scrollUp();
        void scrollDown();
        void scrollToBottom();
        void scrollToTop();
        bool encoder(int change, bool shifted, int threshold);

        void showSelection()
        {
            mShowSelection = true;
        }

        void hideSelection()
        {
            mShowSelection = false;
        }

        void setFocus()
        {
            mHasFocus = true;
        }

        void clearFocus()
        {
            mHasFocus = false;
        }

        int getSelectedIndex()
        {
            return mSelectIndex;
        }

        uint32_t getSelectedPosition();

        void selectNearest(uint32_t position);

    protected:
        Slices *mpSlices = 0;
        int mTextSize = 10;
        int mTopIndex, mSelectIndex;
        int mHeightInLines;
        int mLeftMargin = 2;
        int mEncoderSum = 0;
        bool mShowSelection = true;
        bool mHasFocus = true;

        void sync();
    };

} /* namespace od */

#endif /* APP_GRAPHICS_SLICELIST_H_ */
