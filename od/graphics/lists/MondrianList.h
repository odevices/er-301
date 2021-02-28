/*
 * MondrianList.h
 *
 *  Created on: 1 Sep 2017
 *      Author: clarkson
 */

#ifndef APP_GRAPHICS_MONDRIANLIST_H_
#define APP_GRAPHICS_MONDRIANLIST_H_

#include <od/graphics/Graphic.h>
#include <string>
#include <vector>

namespace od
{

    class MondrianList : public Graphic
    {
    public:
        MondrianList(int left, int bottom, int width, int height);
        virtual ~MondrianList();

        // Describe layout with these methods.
        void addGraphic(Graphic *graphic);
        void setAnchorGraphic(Graphic *graphic);
        void startNewRow();
        void beginSelectableOn();
        void beginSelectableOff();
        void endSelectable();
        void beginJustifyLeft();
        void beginJustifyRight();
        void beginJustifyCenter();
        void endJustify();
        void clear();

        bool scrollUp();
        bool scrollDown();
        void scrollToBottom();
        void scrollToTop();
        void scrollToRow(int index);
        void scrollToGraphic(Graphic *graphic);

        int getCurrentRow();
        int getRowCount();

        bool encoder(int change, bool shifted, int threshold);
        Graphic *selectByButton(int column);
        void setPadding(int horizontal, int vertical);

        void setExpandToPly(bool value)
        {
            mExpandToPly = value;
        }

        void invalidateLayout()
        {
            mLayoutChanged = true;
        }

        void updateLayout();

#ifndef SWIGLUA
        virtual void draw(FrameBuffer &fb);
#endif

    private:
        struct Element
        {
            enum Type
            {
                eGraphic,
                eNewRow,
                // selectable
                eBeginSelectableOn,
                eBeginSelectableOff,
                eEndSelectable,
                // justification
                eBeginJustifyRight,
                eBeginJustifyLeft,
                eBeginJustifyCenter,
                eEndJustify
            };
            Type mType;
            Graphic *mpGraphic = 0;
            int mRowIndex = 0;
            int mLeft = 0;

            Element(Type type) : mType(type)
            {
            }

            Element(Type type, Graphic *graphic) : mType(type), mpGraphic(graphic)
            {
            }
        };

        struct Row
        {
            int mOffset;
            int mBottom;
            int mHeight;
            bool mSelectable;

            Row(int offset, int bottom, int height, bool selectable) : mOffset(offset), mBottom(bottom), mHeight(height), mSelectable(
                                                                                                                              selectable)
            {
            }
        };

        std::vector<Element> mElements;
        std::vector<Row> mRows;
        int mRowIndex = 0;
        int mSelectableRowCount = 0;
        int mEncoderSum = 0;
        int mVerticalPadding = 0;
        int mHorizontalPadding = 0;
        bool mLayoutChanged = false;
        bool mShowSelection = true;
        bool mShowScrollbar = true;
        bool mExpandToPly = true;
        bool mExpandToTallest = true;

        float mOriginState = 0.0f;
        int mBuiltOrigin = 0;
        int mTargetOrigin = 0;
        float mSelectionHeight = 0.0f;

        Graphic *mAnchorGraphic = 0;
        void calculateLayout();
        void updateOrigin(int origin);
        void moveToRow(int index);

        int findSelectableRowAbove(int index);
        int findSelectableRowBelow(int index);
        int addRow(int x, int y, int w, int h, bool selectable,
                   Justification justify);
    };

} /* namespace od */

#endif /* APP_GRAPHICS_MONDRIANLIST_H_ */
