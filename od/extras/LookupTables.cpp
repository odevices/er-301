/*
 * LookupTables.cpp
 *
 *  Created on: 10 Jun 2016
 *      Author: clarkson
 */

#include <od/extras/LookupTables.h>
#include <od/extras/Conversions.h>
#include <math.h>

namespace od
{

#define LUTSIZE 128
#define SINESIZE (4 * LUTSIZE)

#define TWOPI (2.0f * (float)M_PI)

    LookupTable LookupTables::Flat;
    LookupTable LookupTables::FrameOfLinearRamp;
    LookupTable LookupTables::FrameOfCosineRamp;
    LookupTable LookupTables::ConcaveParabolicFall;
    LookupTable LookupTables::ConcaveParabolicRise;
    LookupTable LookupTables::ConvexParabolicFall;
    LookupTable LookupTables::ConvexParabolicRise;
    LookupTable LookupTables::ConvexExponentialRise;
    LookupTable LookupTables::ConcaveExponentialRise;
    LookupTable LookupTables::VoltPerOctave;
    LookupTable LookupTables::ToDecibels;
    LookupTable LookupTables::Sine;

    void LookupTables::initialize()
    {
        int i;
        float x;

        Flat.mValues.resize(2);
        ConcaveParabolicRise.mValues.resize(LUTSIZE);
        ConcaveParabolicFall.mValues.resize(LUTSIZE);
        ConvexParabolicRise.mValues.resize(LUTSIZE);
        ConvexParabolicFall.mValues.resize(LUTSIZE);
        ConvexExponentialRise.mValues.resize(LUTSIZE);
        ConcaveExponentialRise.mValues.resize(LUTSIZE);
        VoltPerOctave.mValues.resize(LUTSIZE);
        ToDecibels.mValues.resize(LUTSIZE);
        Sine.mValues.resize(SINESIZE);

        Flat.mValues[0] = 1.0f;
        Flat.mValues[1] = 1.0f;

        for (i = 0, x = 0; i < LUTSIZE; i++, x += 1.0f / (LUTSIZE - 1))
        {
            ConcaveParabolicRise.mValues[i] = x * x;
            ConcaveParabolicFall.mValues[i] = (x - 1) * (x - 1);
            ConvexParabolicFall.mValues[i] = 1 - x * x;
            ConvexParabolicRise.mValues[i] = 1 - (x - 1) * (x - 1);
            ConvexExponentialRise.mValues[i] = (exp(12 * x) - 1) / (exp(12) - 1);
            ConcaveExponentialRise.mValues[i] = (exp(-12 * x) - 1) / (exp(-12) - 1);
            VoltPerOctave.mValues[i] = powf(2, x);
            ToDecibels.mValues[i] = toDecibels(x);
        }

        FrameOfLinearRamp.mValues.resize(globalConfig.frameLength);
        FrameOfCosineRamp.mValues.resize(globalConfig.frameLength);
        for (i = 0, x = 0; i < (int)globalConfig.frameLength;
             i++, x += 1.0f / (globalConfig.frameLength - 1))
        {
            FrameOfLinearRamp.mValues[i] = x;
            FrameOfCosineRamp.mValues[i] = 0.5f * (1.0f - cosf(0.5f * TWOPI * x));
        }
        // make sure the last value is 1.0
        FrameOfLinearRamp.mValues[globalConfig.frameLength - 1] = 1.0f;
        FrameOfCosineRamp.mValues[globalConfig.frameLength - 1] = 1.0f;
        FrameOfCosineRamp.mValues[0] = 0.0f;

        //  0 to 2pi
        for (i = 0, x = 0; i < SINESIZE; i++, x += 1.0f / (SINESIZE - 1))
        {
            Sine.mValues[i] = sinf(TWOPI * x);
        }
    }

    float LookupTables::sin(float x)
    {
        return Sine.getWrappedInterpolated(x / TWOPI);
    }

} /* namespace od */
