#include <core/objects/random/WhiteNoise.h>
#include <od/extras/Random.h>
#include <od/config.h>

namespace od
{

    WhiteNoise::WhiteNoise()
    {
        addOutput(mOutput);
    }

    WhiteNoise::~WhiteNoise()
    {
    }

    void WhiteNoise::process()
    {
        float *out = mOutput.buffer();
        float *end = out + globalConfig.frameLength;

        while (out < end)
        {
            *(out++) = Random::generateFloat(-1.0f, 1.0f);
        }
    }

} /* namespace od */
