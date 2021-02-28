#pragma once

#include <vector>
#include <string>
#include <od/extras/ReferenceCounted.h>

namespace od
{

    class LongestPath : public ReferenceCounted
    {
    public:
        LongestPath();
        virtual ~LongestPath();

        void add(const char *path);
        std::string calculate();

    private:
        std::vector<std::string> mPaths;
    };

} /* namespace od */
