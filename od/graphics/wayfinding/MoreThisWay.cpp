/*
 * MoreThisWay.cpp
 *
 *  Created on: 12 Dec 2017
 *      Author: clarkson
 */

#include <od/graphics/wayfinding/MoreThisWay.h>

#define HEIGHT 10

namespace od
{

    MoreThisWay::MoreThisWay(Direction direction) : Graphic(0, 0, SECTION_PLY, HEIGHT + 3), mDirection(direction)
    {
    }

    MoreThisWay::~MoreThisWay()
    {
    }

    void MoreThisWay::draw(FrameBuffer &fb)
    {
        Color color2 = sTween * WHITE;
        Color color = WHITE - color2;
        int mid = mWorldLeft + mWidth / 2;

        switch (mDirection)
        {
        case Direction::up:
            fb.line(color, mWorldLeft + 3, mBottom, mid, mBottom + 5);
            fb.line(color, mWorldLeft + mWidth - 3, mBottom, mid, mBottom + 5);
            fb.line(color2, mWorldLeft + 3, mBottom + 3, mid, mBottom + 8);
            fb.line(color2, mWorldLeft + mWidth - 3, mBottom + 3, mid, mBottom + 8);
            break;
        case Direction::left:
            fb.vline(color, mWorldLeft - 3, mBottom, mBottom + 63);
            fb.vline(color2, mWorldLeft - 1, mBottom, mBottom + 63);
            break;
        case Direction::right:
            fb.vline(color2, mWorldLeft + mWidth - 3, mBottom, mBottom + 63);
            fb.vline(color, mWorldLeft + mWidth - 1, mBottom, mBottom + 63);
            break;
        default:
            break;
        }
    }

} /* namespace od */
