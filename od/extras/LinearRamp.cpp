/*
 * LinearRamp.cpp
 *
 *  Created on: Oct 22, 2016
 *      Author: clarkson
 */

#include <od/extras/LinearRamp.h>
#include <od/extras/LookupTables.h>
#include <od/config.h>

namespace od
{

    LinearRamp::LinearRamp()
    {
    }

    LinearRamp::~LinearRamp()
    {
    }

    void LinearRamp::getInterpolatedFrame(float *out)
    {
        if (done())
        {
            for (int i = 0; i < FRAMELENGTH; i++)
            {
                out[i] = mGoal;
            }
        }
        else
        {
            float *ramp = LookupTables::FrameOfLinearRamp.mValues.data();
            for (int i = 0; i < FRAMELENGTH; i++)
            {
                out[i] = mValue + mStep * ramp[i];
            }
        }
    }

} /* namespace od */
