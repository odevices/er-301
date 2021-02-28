/*
 * DeadbandFilter.cpp
 *
 *  Created on: 11 Jul 2016
 *      Author: clarkson
 */

#include <core/objects/filters/DeadbandFilter.h>
#include <od/config.h>
#include <math.h>

namespace od
{

    DeadbandFilter::DeadbandFilter()
    {
        addInput(mInput);
        addOutput(mOutput);
        addParameter(mThreshold);
    }

    DeadbandFilter::~DeadbandFilter()
    {
    }

    void DeadbandFilter::process()
    {
        float *in = mInput.buffer();
        float *out = mOutput.buffer();
        float threshold = mThreshold.value();

        for (int i = 0; i < FRAMELENGTH; i++)
        {
            if (fabs(in[i] - mLast) > threshold)
            {
                mLast = in[i];
            }
            out[i] = mLast;
        }
    }

} // namespace od
/* namespace od */
