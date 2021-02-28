/*
 * Repeater.h
 *
 *  Created on: 21 Aug 2018
 *      Author: clarkson
 */

#ifndef APP_OBJECTS_REPEATER_H_
#define APP_OBJECTS_REPEATER_H_

#include <od/extras/ReferenceCounted.h>
#include <od/objects/Inlet.h>
#include <od/objects/Outlet.h>

namespace od
{

    class Repeater : public ReferenceCounted
    {
    public:
        Repeater();
        virtual ~Repeater();

        void copyInputToOutput();
        void fade(float a, float w0);

        inline void mute()
        {
            mOutlet.mute();
        }

        inline void unmute()
        {
            mOutlet.unmute();
        }

        Inlet mInlet;
        Outlet mOutlet;
    };

} /* namespace od */

#endif /* APP_OBJECTS_REPEATER_H_ */
