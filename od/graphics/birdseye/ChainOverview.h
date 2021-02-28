/*
 * ChainOverview.h
 *
 *  Created on: 28 Jul 2018
 *      Author: clarkson
 */

#ifndef APP_GRAPHICS_BIRDSEYE_CHAINOVERVIEW_H_
#define APP_GRAPHICS_BIRDSEYE_CHAINOVERVIEW_H_

#include <od/graphics/Graphic.h>
#include <od/graphics/birdseye/XPath.h>
#include <string>
#include <vector>
#include <stack>

namespace od
{

    // Uses a layered "icicle" diagram to depict a chain's hierarchy of units and branches.
    class ChainOverview : public Graphic
    {
    public:
        ChainOverview(int left, int bottom, int width, int height);
        virtual ~ChainOverview();

        void fillXPath(XPath &xpath, int id);
        int select(XPath &xpath);
        int select(int id);
        int selectColumn(int col, bool shifted);
        int selected();
        bool left();
        bool right();
        bool up();
        bool down();
        bool encoder(int change, bool shifted, int threshold);

        bool next();
        bool previous();
        void setDepthFirstNavigation(bool value);

        void setName(const std::string &name);

        int startUnit(const std::string &name, const std::string &longName,
                      int index);
        void endUnit();

        int startBranch(const std::string &name, int index);
        int addSource(const std::string &name, int index);
        void endBranch();

        int startPatch(const std::string &name, int index);
        void endPatch();

        void rebuild();
        void clear();

        void setEmptyString(const std::string &text);

#ifndef SWIGLUA
        virtual void draw(FrameBuffer &fb);
#endif

    protected:
        static const int HorizontalMargin = 3;
        static const int VerticalMargin = 3;
        static const int RowHeight = 12;
        static const int RowGap = 5;

        enum ItemType
        {
            topChain,
            unit,
            branch,
            patch,
            source
        };

        struct Item
        {

            Item(ItemType type, const std::string &name, int uid, int index,
                 Item *parent) : mType(type), mName(name), mUID(uid), mExternalIndex(index), mpParent(parent)
            {
                mTextWidth = getStringWidth(name.c_str(), 10);
                mLongTextWidth = 99999;
                if (parent)
                {
                    mDepthIndex = parent->mDepthIndex + 1;
                }
                else
                {
                    mDepthIndex = 0;
                }
            }

            void clear();
            void rebuild(int x, int y);
            void draw(FrameBuffer &fb, int x, int y, int worldLeft,
                      int worldBottom, int worldWidth, int worldHeight);
            Item *find(int id);
            Item *previous();
            Item *next();
            bool intersects(Rect &rect);
            int getChildCount()
            {
                return mChildren.size();
            }

            ItemType mType;
            std::string mName;
            std::string mLongName;
            int mUID;

            std::vector<Item *> mChildren;
            int mChildIndex;
            int mDepthIndex;
            int mExternalIndex;

            int mTextWidth, mLongTextWidth;
            int mWidth, mHeight;
            int mLeft, mBottom;
            int mTextLeft;

            Item *mpParent;
        };

        virtual void notifyHidden();
        virtual void notifyVisible();

    private:
        Item mTop;
        Item *mpCurrentItem;
        std::vector<Item *> mItemById;
        std::string mEmptyString;

        int getItemCount()
        {
            return mItemById.size();
        }

        int mEncoderSum = 0;
        int mTargetX = 0;
        int mTargetY = 0;
        float mCurrentX = 0;
        float mCurrentY = 0;

        bool mDepthFirstNavigation = false;
    };

} /* namespace od */

#endif /* APP_GRAPHICS_BIRDSEYE_CHAINOVERVIEW_H_ */
