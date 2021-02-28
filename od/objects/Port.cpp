/*
 * Port.cpp
 *
 *  Created on: 10 Jun 2016
 *      Author: clarkson
 */

#include <od/objects/Port.h>

namespace od
{

    Port::Port() : mName("Unnamed")
    {
    }

    Port::Port(const std::string &name) : mName(name)
    {
    }

    Port::Port(const std::string &name, uint32_t index) : mName(name), mIndex(index)
    {
    }

    Port::~Port()
    {
    }

    const std::string &Port::name()
    {
        return mName;
    }

} /* namespace od */
