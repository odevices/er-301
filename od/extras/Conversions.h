#pragma once

#include <math.h>
#include <od/config.h>
#include <hal/constants.h>

static inline float toDecibels(float input)
{
    if (input < 1e-8f)
    {
        return -160.0f;
    }
    else
    {
        return 20.0f * log10f(input);
    }
}

static inline float fromDecibels(float input)
{
    if (input < -160.0f)
    {
        return 0.0f;
    }
    else
    {
        return powf(10.0f, input * (1.0f / 20.0f));
    }
}

static inline float toCents(float input)
{
    return input * FULLSCALE_IN_VOLTS * 1200.0f;
}

static inline float fromCents(float input)
{
    return input / (FULLSCALE_IN_VOLTS * 1200.0f);
}

static inline float toPercent(float input)
{
    return input * 100.0f;
}

static inline float fromPercent(float input)
{
    return input * 0.01f;
}
