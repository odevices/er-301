/*
 * XPath.h
 *
 *  Created on: 13 Aug 2018
 *      Author: clarkson
 */

#ifndef APP_GRAPHICS_BIRDSEYE_XPATH_H_
#define APP_GRAPHICS_BIRDSEYE_XPATH_H_

#include <string>
#include <vector>

namespace od
{

    class XPath
    {
    public:
        XPath();
        virtual ~XPath();

        void add(int index);
        void add(const std::string &name, int index);
        void reverse();
        void clear();

        std::string toString(const std::string &separator = "/");
        const std::string &getNodeName(int index);
        int getNodeIndex(int index);
        int getNodeCount();

    protected:
        std::vector<std::string> mNames;
        std::vector<int> mIndices;
    };

} /* namespace od */

#endif /* APP_GRAPHICS_BIRDSEYE_XPATH_H_ */
