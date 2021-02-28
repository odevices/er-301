/*
 * XPath.cpp
 *
 *  Created on: 13 Aug 2018
 *      Author: clarkson
 */

#include <od/graphics/birdseye/XPath.h>
#include <od/extras/Utils.h>

namespace od
{

    XPath::XPath()
    {
    }

    XPath::~XPath()
    {
    }

    void XPath::add(const std::string &node, int index)
    {
        mNames.emplace_back(node);
        mIndices.emplace_back(index);
    }

    void XPath::add(int index)
    {
        mIndices.emplace_back(index);
    }

    void XPath::clear()
    {
        mNames.clear();
        mIndices.clear();
    }

    void XPath::reverse()
    {
        std::reverse(mNames.begin(), mNames.end());
        std::reverse(mIndices.begin(), mIndices.end());
    }

    std::string XPath::toString(const std::string &separator)
    {
        return join(mNames, separator.c_str());
    }

    const std::string &XPath::getNodeName(int index)
    {
        return mNames[index];
    }

    int XPath::getNodeIndex(int index)
    {
        return mIndices[index];
    }

    int XPath::getNodeCount()
    {
        return mIndices.size();
    }

} /* namespace od */
