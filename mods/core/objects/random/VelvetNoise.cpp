#include <core/objects/random/VelvetNoise.h>
#include <od/extras/Random.h>
#include <od/config.h>
#include <hal/ops.h>
#include <string.h>

namespace od
{

    VelvetNoise::VelvetNoise()
    {
        addOutput(mOutput);
        addParameter(mRate);
    }

    VelvetNoise::~VelvetNoise()
    {
    }

    void VelvetNoise::process()
    {
        float *out = mOutput.buffer();
        float rate = MAX(0, mRate.value());
        memset(out, 0, FRAMELENGTH * sizeof(float));
        if (rate > 0.0f)
        {
            float density = rate * globalConfig.samplePeriod;
            float period = 1.0f / density;
            if (mGoal > period)
            {
                mGoal = period;
            }

            for (int i = 0; i < FRAMELENGTH; i++, mPhase++)
            {
                if (mPhase > mGoal)
                {
                    mPhase = 0;
                    float r1 = Random::generateFloat(0.0f, 1.0f);
                    float r2 = Random::generateFloat(0.0f, 1.0f);
                    float s = 2.0f * ((int)(r1 + 0.5f)) - 1.0f;
                    out[i] = s;
                    mGoal = (int)(period * r2 + 0.5f);
                }
            }
        }
    }

} /* namespace od */
