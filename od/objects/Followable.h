#pragma once

#include <od/extras/ReferenceCounted.h>
#include <vector>
#include <string>
#include <limits>

namespace od
{

    class Followable : public ReferenceCounted
    {
    public:
        Followable();
        virtual ~Followable();

        virtual float value() = 0;
        virtual float target() = 0;
        virtual int roundValue() = 0;
        virtual int roundTarget() = 0;
    };

} // namespace od
