#pragma once

#include <od/audio/Sample.h>
#include <od/audio/Slices.h>
#include <od/extras/RealFFT.h>

namespace od
{

    class OnsetDetector
    {
    public:
        OnsetDetector();
        virtual ~OnsetDetector();

        bool computeSpectralFlux(Sample *sample, uint32_t start, uint32_t end,
                                 float targetRate, float overlap);

        // peakThreshold - distance above the local median (units: sigmas)
        // analysisWindow - window size used for local median (units: seconds)
        // coolDownPeriod - minimum period required between peaks (units: seconds)
        int findPeaks(float peakThreshold, float analysisWindow,
                      float coolDownPeriod, Slices *slices = 0,
                      float offset = 0.0f);

        void freeResources();

    protected:
        char *mpBuffer = 0;
        int mLength;
        float mSampleRate = 100;
        float mOriginalSampleRate = 48000;
        uint32_t mStartSample = 0;
    };

} /* namespace od */
