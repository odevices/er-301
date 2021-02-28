/*
 * ChainOverview.cpp
 *
 *  Created on: 28 Jul 2018
 *      Author: clarkson
 */

#include <od/graphics/birdseye/ChainOverview.h>
#include <stdlib.h>
#include <math.h>

namespace od
{

    ChainOverview::ChainOverview(int left, int bottom, int width, int height) : Graphic(left, bottom, width, height), mTop(ItemType::topChain, "top", 0,
                                                                                                                           0, 0)
    {
        clear();
    }

    ChainOverview::~ChainOverview()
    {
        clear();
    }

    void ChainOverview::fillXPath(XPath &xpath, int id)
    {
        xpath.clear();
        if (getItemCount() == 0)
        {
            return;
        }

        id = CLAMP(0, getItemCount() - 1, id);
        Item *item = mItemById[id];

        while (item->mpParent)
        {
            if (item->mType == unit)
            {
                //xpath.add(item->mLongName, item->mExternalIndex);
                xpath.add(item->mExternalIndex);
            }
            else
            {
                //xpath.add(item->mName, item->mExternalIndex);
                xpath.add(item->mExternalIndex);
            }

            item = item->mpParent;
        }

        xpath.reverse();
    }

    void ChainOverview::setDepthFirstNavigation(bool value)
    {
        mDepthFirstNavigation = value;
    }

    int ChainOverview::select(int id)
    {
        if (id >= 0 || id < getItemCount())
        {
            mpCurrentItem = mItemById[id];
        }

        return selected();
    }

    int ChainOverview::select(XPath &xpath)
    {
        select(0);

        int n = xpath.getNodeCount();
        for (int i = 0; i < n; i++)
        {
            int nodeIndex = xpath.getNodeIndex(i);
            for (Item *child : mpCurrentItem->mChildren)
            {
                if (child->mExternalIndex == nodeIndex)
                {
                    mpCurrentItem = child;
                    break;
                }
                else if (child->mExternalIndex > nodeIndex)
                {
                    return selected();
                }
            }
        }
        return selected();
    }

    int ChainOverview::selected()
    {
        return mpCurrentItem->mUID;
    }

    bool ChainOverview::left()
    {

        if (mpCurrentItem->mUID > 0)
        {
            for (int i = mpCurrentItem->mUID - 1; i >= 0; i--)
            {
                Item *item = mItemById[i];
                if (item->mDepthIndex == mpCurrentItem->mDepthIndex)
                {
                    mpCurrentItem = item;
                    return true;
                }
            }

            return false;
        }
        else
        {
            return false;
        }
    }

    bool ChainOverview::right()
    {
        for (int i = mpCurrentItem->mUID + 1; i < getItemCount(); i++)
        {
            Item *item = mItemById[i];
            if (item->mDepthIndex == mpCurrentItem->mDepthIndex)
            {
                mpCurrentItem = item;
                return true;
            }
        }

        return false;
    }

    bool ChainOverview::up()
    {
        if (mpCurrentItem->mUID > 1 && mpCurrentItem->mpParent)
        {
            mpCurrentItem = mpCurrentItem->mpParent;
            return true;
        }

        return false;
    }

    bool ChainOverview::down()
    {
        if (mpCurrentItem->mChildren.size() > 0)
        {
            mpCurrentItem = mpCurrentItem->mChildren[0];
            return true;
        }
        return false;
    }

    bool ChainOverview::next()
    {
        if (mpCurrentItem->mUID + 1 < getItemCount())
        {
            mpCurrentItem = mItemById[mpCurrentItem->mUID + 1];
            return true;
        }

        return false;
    }

    bool ChainOverview::previous()
    {
        if (mpCurrentItem->mUID > 1)
        {
            mpCurrentItem = mItemById[mpCurrentItem->mUID - 1];
            return true;
        }

        return false;
    }

    bool ChainOverview::encoder(int change, bool shifted, int threshold)
    {
        mEncoderSum += change;
        if (mEncoderSum > threshold)
        {
            mEncoderSum = 0;
            if (shifted)
            {
                return down();
            }
            else if (mDepthFirstNavigation)
            {
                return next();
            }
            else
            {
                return right();
            }
        }
        else if (mEncoderSum < -threshold)
        {
            mEncoderSum = 0;
            if (shifted)
            {
                return up();
            }
            else if (mDepthFirstNavigation)
            {
                return previous();
            }
            else
            {
                return left();
            }
        }

        return false;
    }

    void ChainOverview::setName(const std::string &name)
    {
        mTop.mName = name;
    }

    int ChainOverview::startUnit(const std::string &name,
                                 const std::string &longName, int index)
    {
        // Are we inside a patch or branch?
        if (mpCurrentItem->mType == ItemType::unit)
        {
            return -1;
        }

        Item *item = new Item(unit, name, getItemCount(), index, mpCurrentItem);
        mItemById.push_back(item);
        item->mChildIndex = mpCurrentItem->mChildren.size();
        item->mLongName = longName;
        item->mLongTextWidth = getStringWidth(longName.c_str(), 10);
        mpCurrentItem->mChildren.push_back(item);
        mpCurrentItem = item;
        return item->mUID;
    }

    void ChainOverview::endUnit()
    {
        if (mpCurrentItem->mType == ItemType::unit)
        {
            mpCurrentItem = mpCurrentItem->mpParent;
        }
    }

    int ChainOverview::startBranch(const std::string &name, int index)
    {
        // Are we inside a unit?
        if (mpCurrentItem->mType != ItemType::unit)
        {
            return -1;
        }

        Item *item = new Item(branch, name, getItemCount(), index, mpCurrentItem);
        mItemById.push_back(item);
        item->mChildIndex = mpCurrentItem->mChildren.size();
        mpCurrentItem->mChildren.push_back(item);
        mpCurrentItem = item;
        return item->mUID;
    }

    int ChainOverview::addSource(const std::string &name, int index)
    {
        if (mpCurrentItem->mType == ItemType::topChain || mpCurrentItem->mType == ItemType::branch)
        {
            Item *item = new Item(source, name, getItemCount(), index,
                                  mpCurrentItem);
            mItemById.push_back(item);
            item->mChildIndex = mpCurrentItem->mChildren.size();
            mpCurrentItem->mChildren.push_back(item);
            return item->mUID;
        }
        else
        {
            return -1;
        }
    }

    void ChainOverview::endBranch()
    {
        if (mpCurrentItem->mType == ItemType::branch)
        {
            mpCurrentItem = mpCurrentItem->mpParent;
        }
    }

    int ChainOverview::startPatch(const std::string &name, int index)
    {
        // Are we inside a unit?
        if (mpCurrentItem->mType != ItemType::unit)
        {
            return -1;
        }

        Item *item = new Item(patch, name, getItemCount(), index, mpCurrentItem);
        mItemById.push_back(item);
        item->mChildIndex = mpCurrentItem->mChildren.size();
        mpCurrentItem->mChildren.push_back(item);
        mpCurrentItem = item;
        return item->mUID;
    }

    void ChainOverview::endPatch()
    {
        if (mpCurrentItem->mType == ItemType::patch)
        {
            mpCurrentItem = mpCurrentItem->mpParent;
        }
    }

    void ChainOverview::clear()
    {
        mpCurrentItem = &mTop;
        mTop.clear();
        mItemById.clear();
        mItemById.push_back(&mTop);
    }

    void ChainOverview::Item::clear()
    {
        for (Item *item : mChildren)
        {
            item->clear();
            delete item;
        }
        mChildren.clear();
    }

    void ChainOverview::Item::rebuild(int x, int y)
    {
        mTextLeft = mLeft = x;
        mBottom = y;
        mWidth = 0;
        mHeight = RowHeight;

        int childrenBottom = mBottom - RowHeight;

        for (Item *item : mChildren)
        {
            item->rebuild(mLeft + mWidth, childrenBottom);
            mWidth += item->mWidth;
            mHeight = MAX(mHeight, RowHeight + item->mHeight);
        }

        if (mWidth < mTextWidth + 2 * HorizontalMargin)
        {
            mWidth = mTextWidth + 2 * HorizontalMargin;
        }

        if (mType == branch || mType == patch)
        {
            mWidth += HorizontalMargin;
        }
    }

    void ChainOverview::rebuild()
    {
        mTop.rebuild(0, -RowHeight);
    }

    void ChainOverview::Item::draw(FrameBuffer &fb, int x, int y, int worldLeft,
                                   int worldBottom, int worldWidth, int worldHeight)
    {
        int x0 = mLeft + x;
        int y0 = mBottom + y;

        if (y0 + RowHeight < 0 || y0 > 64 || x0 > 256 || x0 + mWidth < 0)
        {
            // This item is not visible.
        }
        else
        {
            int visibleWidth = MIN(worldLeft + worldWidth,
                                   x0 + mWidth) -
                               MAX(worldLeft, x0);
            visibleWidth = MAX(0, visibleWidth);

            int textWidth;
            const char *text;
            if (mLongTextWidth + 2 * HorizontalMargin < visibleWidth)
            {
                textWidth = mLongTextWidth;
                text = mLongName.c_str();
            }
            else
            {
                textWidth = mTextWidth;
                text = mName.c_str();
            }

            int textLeft = CLAMP_UNORDERED(
                worldLeft, x0 + mWidth - 2 * HorizontalMargin - textWidth, x0);
            textLeft += HorizontalMargin;
            mTextLeft = textLeft;

            switch (mType)
            {
            case topChain:
                break;
            case unit:
                fb.fill(GRAY3, x0 + 1, y0 + 1, x0 + mWidth - 1, y0 + RowHeight - 1);
                //fb.box(WHITE, x0, y0, x0 + mWidth, y0 + RowHeight);
                fb.drawBeveledBox(WHITE, x0, y0, x0 + mWidth, y0 + RowHeight);
                fb.text(WHITE, textLeft, y0 + VerticalMargin, text, 10);
                break;
            case branch:
            case patch:
                fb.text(GRAY11, textLeft, y0 + VerticalMargin, text, 10);
                break;
            case source:
                fb.fill(GRAY3, x0 + 1, y0 + 1, x0 + mWidth - 1, y0 + RowHeight - 1);
                fb.text(GRAY11, textLeft, y0 + VerticalMargin, text, 10);
                break;
            }
        }

        if (y0 + RowHeight < 0 || x0 > 256)
        {
            // Children are not visible.
        }
        else
        {
            for (Item *item : mChildren)
            {
                item->draw(fb, x, y, worldLeft, worldBottom, worldWidth,
                           worldHeight);
            }
        }
    }

    void ChainOverview::draw(FrameBuffer &fb)
    {
        fb.vline(GRAY1, COL2_LEFT, mWorldBottom, mWorldBottom + mHeight);
        fb.vline(GRAY1, COL3_LEFT, mWorldBottom, mWorldBottom + mHeight);
        fb.vline(GRAY1, COL4_LEFT, mWorldBottom, mWorldBottom + mHeight);
        fb.vline(GRAY1, COL5_LEFT, mWorldBottom, mWorldBottom + mHeight);
        fb.vline(GRAY1, COL6_LEFT, mWorldBottom, mWorldBottom + mHeight);

        int x = mpCurrentItem->mLeft;
        int y = mpCurrentItem->mBottom;
        int xc = mWorldLeft + mWidth / 2;
        int yc = mWorldBottom + mHeight / 2;
        int x0 = CLAMP(mWidth / 2 - 5, MAX(mWidth / 2, mTop.mWidth - mWidth / 2) + 5,
                       x);
        int h = mpCurrentItem->mHeight - y - RowHeight;
        int y0 = -CLAMP(mHeight / 2 - 5, MAX(mHeight / 2, h - mHeight / 2) + 5, -y);

        // Selection point
        mTargetX = xc - x0;
        mTargetY = yc - y0;

        mCurrentX = 0.8f * mCurrentX + 0.2f * mTargetX;
        mCurrentY = 0.8f * mCurrentY + 0.2f * mTargetY;

        if (fabs(mCurrentX - mTargetX) < 2)
        {
            mCurrentX = mTargetX;
        }

        if (fabs(mCurrentY - mTargetY) < 2)
        {
            mCurrentY = mTargetY;
        }

        if (mTop.mChildren.size() == 0)
        {
            int w = getStringWidth(mEmptyString.c_str(), 10);
            fb.text(WHITE, mWorldLeft + (mWidth - w) / 2,
                    mWorldBottom + mHeight / 2 - 6, mEmptyString.c_str(), 10);
        }
        else
        {
            // Draw the hierarchy such that the selection point is in the middle of the view.
            mTop.draw(fb, mCurrentX, mCurrentY, mWorldLeft, mWorldBottom, mWidth,
                      mHeight);
        }

        mCursorState.x = x + mTargetX + HorizontalMargin + mpCurrentItem->mTextWidth / 2;
        mCursorState.y = y + mTargetY + RowHeight + VerticalMargin;
        mCursorState.orientation = cursorDown;
    }

    void ChainOverview::notifyHidden()
    {
    }

    void ChainOverview::notifyVisible()
    {
    }

    ChainOverview::Item *ChainOverview::Item::find(int id)
    {
        for (Item *item : mChildren)
        {
            if (item->mUID == id)
            {
                return item;
            }
        }

        for (Item *item : mChildren)
        {
            Item *found = item->find(id);
            if (found)
            {
                return found;
            }
        }

        return 0;
    }

    ChainOverview::Item *ChainOverview::Item::previous()
    {
        if (mpParent == 0 || mChildIndex == 0)
        {
            return 0;
        }

        return mpParent->mChildren[mChildIndex - 1];
    }

    ChainOverview::Item *ChainOverview::Item::next()
    {
        if (mpParent == 0 || mChildIndex + 1 == mpParent->getChildCount())
        {
            return 0;
        }

        return mpParent->mChildren[mChildIndex + 1];
    }

    int ChainOverview::selectColumn(int col, bool shifted)
    {
        Rect rect;
        rect.bottom = -9999;
        rect.top = 9999;
        switch (col)
        {
        case 0:
            rect.left = COL1_LEFT;
            rect.right = COL2_LEFT;
            break;
        case 1:
            rect.left = COL2_LEFT;
            rect.right = COL3_LEFT;
            break;
        case 2:
            rect.left = COL3_LEFT;
            rect.right = COL4_LEFT;
            break;
        case 3:
            rect.left = COL4_LEFT;
            rect.right = COL5_LEFT;
            break;
        case 4:
            rect.left = COL5_LEFT;
            rect.right = COL6_LEFT;
            break;
        case 5:
        default:
            rect.left = COL6_LEFT;
            rect.right = 256;
            break;
        }

        int choice = selected();
        int minDistance = 9999;

        if (mpCurrentItem->intersects(rect))
        {
            return down();
        }

        for (Item *item : mItemById)
        {
            if (item->intersects(rect))
            {
                int distance = abs(item->mDepthIndex - mpCurrentItem->mDepthIndex);
                if (distance < minDistance)
                {
                    choice = item->mUID;
                }
            }
        }

        return select(choice);
    }

    bool ChainOverview::Item::intersects(Rect &r)
    {
        int mid = mTextLeft + mTextWidth / 2;
        return !(r.left > mid + 1 || r.right < mid - 1);
    }

    void ChainOverview::setEmptyString(const std::string &text)
    {
        mEmptyString = text;
    }

} /* namespace od */
