#pragma once

#include <od/graphics/Graphic.h>
#include <string>
#include <vector>

namespace od
{

    class MenuArc : public Graphic
    {
    public:
        MenuArc();
        virtual ~MenuArc();

#ifndef SWIGLUA
        virtual void draw(FrameBuffer &fb);
#endif

        void setAnchor(int x);

        void clear();
        void add(const std::string &text);
        int size();

        bool select(const std::string &name);
        bool select(int index);
        const std::string &selectedText();
        int selectedIndex();

        void scrollUp();
        void scrollDown();
        void scrollToBottom();
        void scrollToTop();
        bool encoder(int change, bool shifted, int threshold);

    private:
        struct Item
        {
            Item(const std::string &text);
            void draw(FrameBuffer &fb, int left, int mid, bool leftJustified, bool selected);
            std::string text;
            int width;
            int selectedWidth;
            int x;
            int y;
        };
        std::vector<Item> mContents;
        int mSelectedIndex = 0;
        float mRenderIndex = 0;
        float mBuiltIndex = -1;
        int mEncoderSum = 0;
        int mAnchor = 128;
        int mMaxTextWidth = 0;
        bool mLeftJustified = true;
        const int cRadius = 24;
    };

} /* namespace od */
