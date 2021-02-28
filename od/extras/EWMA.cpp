/*
 * EWMA.cpp
 *
 *  Created on: Jun 21, 2016
 *      Author: clarkson
 */

#include <od/extras/EWMA.h>
#include <math.h>

namespace od
{

    EWMA::EWMA()
    {
    }

    EWMA::~EWMA()
    {
    }

    void EWMA::setTimeConstantExact(float sampleRate, float secs)
    {
        mAlpha = 1 - expf(-1.0f / (sampleRate * secs));
    }

} /* namespace od */
