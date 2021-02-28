#include <od/objects/heads/Head.h>
#include <limits>

namespace od
{

    Head::Head()
    {
        addParameter(mSampleDuration);
    }

    Head::~Head()
    {
        if (mpSample)
            mpSample->release();
    }

    void Head::setSample(Sample *sample)
    {
        Sample *pSample = mpSample;
        // Prevent audio code from accessing the current sample.
        mpSample = 0;

        if (pSample)
            pSample->release();
        pSample = sample;
        if (pSample)
        {
            pSample->attach();
            // HACK (v0.3.22).  Prevent pop at the end of a sample with non-zero beginning.
            // Why does this work?
            mEndIndex = pSample->mSampleCount - 2;
            mSampleDuration.hardSet(pSample->mTotalSeconds);
        }
        else
        {
            mEndIndex = 0;
            mSampleDuration.hardSet(0);
        }
        mCurrentIndex = 0;

        // Allow audio code to access the new sample.
        mpSample = pSample;
    }

} /* namespace od */
