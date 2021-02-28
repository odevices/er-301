/*
 * MoreThisWay.h
 *
 *  Created on: 12 Dec 2017
 *      Author: clarkson
 */

#ifndef APP_GRAPHICS_WAYFINDING_MORETHISWAY_H_
#define APP_GRAPHICS_WAYFINDING_MORETHISWAY_H_

#include <od/graphics/Graphic.h>

namespace od
{

    class MoreThisWay : public Graphic
    {
    public:
        MoreThisWay(Direction direction);
        virtual ~MoreThisWay();

#ifndef SWIGLUA
        virtual void draw(FrameBuffer &fb);
#endif

    private:
        Direction mDirection;
    };

} /* namespace od */

#endif /* APP_GRAPHICS_WAYFINDING_MORETHISWAY_H_ */
