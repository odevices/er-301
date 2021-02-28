#pragma once

#include <core/objects/granular/Grain.h>

namespace od
{

    class StereoGrain : public Grain
    {
    public:
        StereoGrain();
        virtual ~StereoGrain();

        void synthesize(float *left, float *right);

    protected:
        inline void incrementPhase();

        float mFifoL[3] = {0, 0, 0};
        float mFifoR[3] = {0, 0, 0};
    };

} /* namespace od */
