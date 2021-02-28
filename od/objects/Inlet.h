/*
 * Inlet.h
 *
 *  Created on: 10 Jun 2016
 *      Author: clarkson
 */

#ifndef APP_OBJECTS_INLET_H_
#define APP_OBJECTS_INLET_H_

#include <od/objects/Port.h>
#include <od/objects/Outlet.h>

namespace od
{

    class Inlet : public Port
    {
    public:
#ifndef SWIGLUA
        Inlet();
        Inlet(const std::string &name);
        Inlet(const std::string &name, uint32_t index);
        virtual ~Inlet();

        float *buffer();
        void connect(Outlet *outlet);
        void disconnect();
        bool isConnected();
        bool isConstant();

        Outlet *mInwardConnection = 0;
#endif
    };

} /* namespace od */

#endif /* APP_OBJECTS_INLET_H_ */
