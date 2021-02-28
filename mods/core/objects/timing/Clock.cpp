#include <core/objects/timing/Clock.h>

namespace od
{

    Clock::Clock()
    {
        addInput(mSync);
        addOutput(mOutput);
        addParameter(mDivider);
        addParameter(mMultiplier);
        addParameter(mPulseWidth);
        addOption(mRational);
    }

    Clock::~Clock()
    {
    }

    void Clock::process()
    {
        float *out = mOutput.buffer();
        float *sync = mSync.buffer();

        float periodInSeconds = getPeriod();
        if (mRational.value())
        {
            periodInSeconds *= MAX(1, mDivider.roundTarget());
            periodInSeconds /= MAX(1, mMultiplier.roundTarget());
        }
        else
        {
            periodInSeconds *= MAX(1, mDivider.value());
            periodInSeconds /= MAX(1, mMultiplier.value());
        }

        uint32_t period = periodInSeconds * globalConfig.sampleRate;
        uint32_t high = (periodInSeconds * mPulseWidth.value()) * globalConfig.sampleRate;

        for (int i = 0; i < FRAMELENGTH; i++)
        {
            if (sync[i] > 0.0f)
            {
                mCurrentValue = 1.0f;
                mState = 1;
                mPhase = 0;
            }

            out[i] = mCurrentValue;

            switch (mState)
            {
            case 0:
                if (mPhase >= period)
                {
                    mCurrentValue = 1.0f;
                    mState = 1;
                    mPhase = 0;
                }
                break;
            case 1:
                if (mPhase >= high)
                {
                    mCurrentValue = 0.0f;
                    mState = 0;
                }
                break;
            }

            mPhase++;
        }
    }

} /* namespace od */
