/*
 * MondrianList.cpp
 *
 *  Created on: 1 Sep 2017
 *      Author: clarkson
 */

#include <od/graphics/lists/MondrianList.h>
#include <math.h>

namespace od
{

    MondrianList::MondrianList(int left, int bottom, int width, int height) : Graphic(left, bottom, width, height)
    {
        moveToRow(0);
    }

    MondrianList::~MondrianList()
    {
    }

    void MondrianList::addGraphic(Graphic *graphic)
    {
        mLayoutChanged = true;
        Graphic::addChild(graphic);
        mElements.emplace_back(Element::eGraphic, graphic);
    }

    void MondrianList::clear()
    {
        for (Element &element : mElements)
        {
            if (element.mType == Element::eGraphic)
            {
                Graphic::removeChild(element.mpGraphic);
            }
        }

        mElements.clear();
        mRows.clear();
        mRowIndex = 0;
        mSelectableRowCount = 0;
    }

    void MondrianList::startNewRow()
    {
        if (mElements.size() > 0)
        {
            mElements.emplace_back(Element::eNewRow);
            mLayoutChanged = true;
        }
    }

    void MondrianList::beginSelectableOn()
    {
        mElements.emplace_back(Element::eBeginSelectableOn);
        mLayoutChanged = true;
    }

    void MondrianList::beginSelectableOff()
    {
        mElements.emplace_back(Element::eBeginSelectableOff);
        mLayoutChanged = true;
    }

    void MondrianList::endSelectable()
    {
        mElements.emplace_back(Element::eEndSelectable);
        mLayoutChanged = true;
    }

    void MondrianList::beginJustifyLeft()
    {
        mElements.emplace_back(Element::eBeginJustifyLeft);
        mLayoutChanged = true;
    }

    void MondrianList::beginJustifyRight()
    {
        mElements.emplace_back(Element::eBeginJustifyRight);
        mLayoutChanged = true;
    }

    void MondrianList::beginJustifyCenter()
    {
        mElements.emplace_back(Element::eBeginJustifyCenter);
        mLayoutChanged = true;
    }

    void MondrianList::endJustify()
    {
        mElements.emplace_back(Element::eEndJustify);
        mLayoutChanged = true;
    }

    void MondrianList::draw(FrameBuffer &fb)
    {
        mOriginState = mOriginState * 0.8f + 0.2f * mTargetOrigin;
        if (fabs(mOriginState - mTargetOrigin) < 2)
        {
            mOriginState = mTargetOrigin;
        }
        int origin = mOriginState;

        if (mLayoutChanged)
        {
            updateLayout();
        }
        else if (origin != mBuiltOrigin)
        {
            updateOrigin(origin);
        }

        Graphic::draw(fb);

        int right = mWorldLeft + mWidth;

        if (mShowSelection && mSelectableRowCount > 1)
        {
            int targetHeight = mRows[mRowIndex].mHeight;
            mSelectionHeight = mSelectionHeight * 0.8f + 0.2f * targetHeight;
            if (fabs(mSelectionHeight - targetHeight) < 2)
            {
                mSelectionHeight = targetHeight;
            }
            int h = (int)mSelectionHeight;

            int bottom = mWorldBottom + mHeight / 2 - h / 2;
            int top = bottom + h - 1 - mVerticalPadding;
            fb.fill(GRAY3, mWorldLeft, bottom, right, top);
            fb.hline(WHITE, mWorldLeft, right, bottom - 1);
            fb.hline(WHITE, mWorldLeft, right, top + 1);

            mCursorState.orientation = cursorRight;
            mCursorState.x = mWorldLeft;
            mCursorState.y = (bottom + top) / 2;
        }

        if (mShowScrollbar && mSelectableRowCount > 1)
        {
            // draw scroll bar
            float scrollSize, scrollPos;
            scrollSize = 1.0f / (float)mRows.size();
            scrollPos = mRowIndex / (float)mRows.size();
            int scroll0, scroll1;
            scroll0 = mWorldBottom + mHeight - scrollPos * mHeight;
            scroll1 = scroll0 - scrollSize * mHeight;
            fb.clear(right - 2, mWorldBottom, right, mWorldBottom + mHeight);
            //fb.vline(mForeground, right - 2, scroll0, scroll1);
            fb.vline(mForeground, right - 1, scroll0, scroll1);
        }
    }

    void MondrianList::setPadding(int horizontal, int vertical)
    {
        mLayoutChanged = true;
        mHorizontalPadding = horizontal;
        mVerticalPadding = vertical;
    }

    int MondrianList::addRow(int x, int y, int w, int h, bool selectable,
                             Justification justify)
    {
        int offset;
        switch (justify)
        {
        case Justification::justifyLeft:
            offset = 0;
            break;
        case Justification::justifyRight:
            offset = w - x;
            break;
        case Justification::justifyCenter:
            offset = (w - x) / 2;
            break;
        default:
            offset = 0;
            break;
        }
        h += mVerticalPadding;
        y -= h;
        mRows.emplace_back(offset, y, h, selectable);
        if (selectable)
        {
            mSelectableRowCount++;
        }
        return y;
    }

    template <typename T, T defaultValue>
    class AttributeStack
    {
    public:
        void push(T value)
        {
            mData.push_back(mValue);
            mValue = value;
        }

        void pop()
        {
            if (mData.size() > 0)
            {
                mValue = mData.back();
                mData.pop_back();
            }
        }

        T operator()()
        {
            return mValue;
        }

    private:
        std::vector<T> mData;
        T mValue = defaultValue;
    };

    void MondrianList::calculateLayout()
    {
        AttributeStack<Justification, Justification::justifyRight> justify;
        AttributeStack<bool, true> selectable;
        int i = 0, x = 0, y = 0, h = 0, w = mWidth - 2;
        mRows.clear();
        mSelectableRowCount = 0;
        if (mElements.size() > 0)
        {
            for (Element &element : mElements)
            {
                switch (element.mType)
                {
                case Element::eGraphic:
                {
                    Graphic *g = element.mpGraphic;
                    if (mExpandToPly && selectable())
                    {
                        g->expandToPly(mHorizontalPadding);
                    }
                    if (x + g->mWidth > w)
                    {
                        y = addRow(x, y, w, h, selectable(), justify());
                        // start new row
                        x = 0;
                        h = 0;
                        i++;
                    }
                    element.mLeft = x;
                    element.mRowIndex = i;
                    x += g->mWidth + mHorizontalPadding;
                    if (h < g->mHeight)
                    {
                        h = g->mHeight;
                    }
                }
                break;
                case Element::eNewRow:
                    if (h > 0)
                    {
                        y = addRow(x, y, w, h, selectable(), justify());
                        x = 0;
                        h = 0;
                        i++;
                    }
                    else
                    {
                        // No graphics, so just reset the column.
                        x = 0;
                    }
                    break;
                case Element::eBeginSelectableOn:
                    selectable.push(true);
                    break;
                case Element::eBeginSelectableOff:
                    selectable.push(false);
                    break;
                case Element::eEndSelectable:
                    selectable.pop();
                    break;
                case Element::eBeginJustifyRight:
                    justify.push(Justification::justifyRight);
                    break;
                case Element::eBeginJustifyLeft:
                    justify.push(Justification::justifyLeft);
                    break;
                case Element::eBeginJustifyCenter:
                    justify.push(Justification::justifyCenter);
                    break;
                case Element::eEndJustify:
                    justify.pop();
                    break;
                }
            }
            if (h > 0)
            {
                y = addRow(x, y, w, h, selectable(), justify());
            }
        }

        if (mExpandToTallest && getRowCount() > 0)
        {
            // Expand the height of each row element to match the row height.
            for (Element &element : mElements)
            {
                if (element.mType == Element::eGraphic)
                {
                    Row &row = mRows[element.mRowIndex];
                    Graphic *g = element.mpGraphic;
                    g->setSize(g->mWidth, row.mHeight - mVerticalPadding, false);
                }
            }
        }
        if (mAnchorGraphic)
        {
            scrollToGraphic(mAnchorGraphic);
        }
        else
        {
            scrollToTop();
        }
    }

    void MondrianList::moveToRow(int index)
    {
        if (index < 0)
        {
            mRowIndex = 0;
        }
        else if (index >= getRowCount())
        {
            mRowIndex = getRowCount() - 1;
        }
        else
        {
            mRowIndex = index;
        }

        if (getRowCount() > 0)
        {
            mTargetOrigin = mRows[mRowIndex].mBottom + mRows[mRowIndex].mHeight / 2 - mHeight / 2;
        }
    }

    void MondrianList::updateOrigin(int origin)
    {
        mBuiltOrigin = origin;
        if (getRowCount() > 0)
        {
            for (Element &element : mElements)
            {
                if (element.mType == Element::eGraphic)
                {
                    Row &row = mRows[element.mRowIndex];
                    Graphic *g = element.mpGraphic;
                    g->setPosition(element.mLeft + row.mOffset,
                                   row.mBottom - origin); // - row.mHeight / 2);
                }
            }
        }
    }

    int MondrianList::findSelectableRowAbove(int index)
    {
        for (int i = index - 1; i >= 0; i--)
        {
            if (mRows[i].mSelectable)
            {
                return i;
            }
        }
        return index;
    }

    int MondrianList::findSelectableRowBelow(int index)
    {
        int N = getRowCount();
        for (int i = index + 1; i < N; i++)
        {
            if (mRows[i].mSelectable)
            {
                return i;
            }
        }
        return index;
    }

    bool MondrianList::scrollUp()
    {
        if (getRowCount() == 0)
            return false;
        int index = findSelectableRowAbove(mRowIndex);
        if (mRowIndex != index)
        {
            moveToRow(index);
            return true;
        }
        return false;
    }

    bool MondrianList::scrollDown()
    {
        if (getRowCount() == 0)
            return false;
        int index = findSelectableRowBelow(mRowIndex);
        if (mRowIndex != index)
        {
            moveToRow(index);
            return true;
        }
        return false;
    }

    void MondrianList::scrollToBottom()
    {
        if (getRowCount() == 0)
            return;
        int index = getRowCount() - 1;
        if (!mRows[index].mSelectable)
        {
            index = findSelectableRowAbove(index);
        }
        moveToRow(index);
    }

    void MondrianList::scrollToTop()
    {
        if (getRowCount() == 0)
            return;
        int index = 0;
        if (!mRows[index].mSelectable)
        {
            index = findSelectableRowBelow(index);
        }
        moveToRow(index);
    }

    void MondrianList::scrollToRow(int index)
    {
        if (getRowCount() == 0)
            return;
        if (index < 0)
        {
            index = 0;
        }
        else if (index >= getRowCount())
        {
            index = getRowCount() - 1;
        }
        if (!mRows[index].mSelectable)
        {
            index = findSelectableRowAbove(index);
        }
        if (!mRows[index].mSelectable)
        {
            index = findSelectableRowBelow(index);
        }
        moveToRow(index);
    }

    void MondrianList::scrollToGraphic(Graphic *graphic)
    {
        // Expand the height of each row element to match the row height.
        for (Element &element : mElements)
        {
            if (element.mType == Element::eGraphic && element.mpGraphic == graphic)
            {
                moveToRow(element.mRowIndex);
            }
        }
    }

    bool MondrianList::encoder(int change, bool shifted, int threshold)
    {
        mEncoderSum += change;
        if (mEncoderSum > threshold)
        {
            mEncoderSum = 0;
            scrollDown();
            return true;
        }
        else if (mEncoderSum < -threshold)
        {
            mEncoderSum = 0;
            scrollUp();
            return true;
        }
        return false;
    }

    Graphic *MondrianList::selectByButton(int button)
    {
        int centers[] = {BUTTON1_CENTER,
                         BUTTON2_CENTER,
                         BUTTON3_CENTER,
                         BUTTON4_CENTER,
                         BUTTON5_CENTER,
                         BUTTON6_CENTER};
        Rect target;
        target.bottom = mWorldBottom + mHeight / 2 - 1;
        target.top = target.bottom + 2;

        if (button < 0)
        {
            target.left = centers[0] - 1;
        }
        if (button > 5)
        {
            target.left = centers[5] - 1;
        }
        else
        {
            target.left = centers[button] - 1;
        }
        target.right = target.left + 2;

        for (Graphic *graphic : mChildren)
        {
            if (graphic->intersects(target))
            {
                mAnchorGraphic = graphic;
                return graphic;
            }
        }

        mAnchorGraphic = 0;
        return 0;
    }

    void MondrianList::setAnchorGraphic(Graphic *graphic)
    {
        mAnchorGraphic = graphic;
        scrollToGraphic(graphic);
    }

    int MondrianList::getRowCount()
    {
        return (int)mRows.size();
    }

    int MondrianList::getCurrentRow()
    {
        return mRowIndex;
    }

    void MondrianList::updateLayout()
    {
        calculateLayout();
        updateOrigin(mOriginState);
        mLayoutChanged = false;
    }

} /* namespace od */
