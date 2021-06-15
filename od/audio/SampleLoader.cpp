#include <od/audio/SampleLoader.h>
#include <od/audio/WavFileReader.h>

namespace od
{

    SampleLoader::SampleLoader()
    {
    }

    SampleLoader::~SampleLoader()
    {
        if (mpSample)
        {
            mpSample->release();
            mpSample = NULL;
        }

        if (mpLoadInfo)
        {
            mpLoadInfo->release();
            mpLoadInfo = NULL;
        }
    }

    bool SampleLoader::set(Sample *sample, SampleLoadInfo *info)
    {
        if (mStatus == STATUS_WORKING || sample == 0 || info == 0)
            return false;

        mpSample = sample;
        mpSample->attach();
        mpLoadInfo = info;
        mpLoadInfo->attach();
        mStatus = STATUS_WORKING;
        return true;
    }

    void SampleLoader::work()
    {
        if (mpSample)
        {
            mStatus = load();
            mpSample->release();
            mpSample = NULL;
        }

        if (mpLoadInfo)
        {
            mpLoadInfo->release();
            mpLoadInfo = NULL;
        }
    }

    int SampleLoader::load()
    {
        const int SamplesPerBlock = 16 * 1024;
        float *buffer = mpSample->mpData;

        if (buffer == NULL)
        {
            return STATUS_SAMPLE_NOT_PREPARED;
        }

        mSamplesRemaining = mpLoadInfo->mSampleCount;
        if (mSamplesRemaining != mpSample->mSampleCount || mpLoadInfo->mChannelCount != (int)mpSample->mChannelCount)
        {
            return STATUS_SAMPLE_NOT_PREPARED;
        }

        mPercentDone = 0.0f;
        mSamplesRead = 0;

        float B = 100.0f / (mpLoadInfo->mSampleCount / SamplesPerBlock + 1);
        for (SampleLoadInfo::Entry &entry : mpLoadInfo->mEntries)
        {
            WavFileReader reader;
            int sr;

            if (!reader.open(entry.filename))
            {
                return STATUS_ERROR_OPENING_FILE;
            }

            int samplesRemaining = entry.sampleCount;
            while (samplesRemaining)
            {
                if (mCancelRequested)
                {
                    return STATUS_CANCELED;
                }

                if (samplesRemaining < SamplesPerBlock)
                    sr = samplesRemaining;
                else
                    sr = SamplesPerBlock;

                if (reader.readSamples(buffer, sr) != (uint32_t)sr)
                {
                    return STATUS_ERROR_READING_FILE;
                }
                else
                {
                    if (reader.getChannelCount() == 1 && mpSample->mChannelCount == 2)
                    {
                        // This sample is allocated as stereo but the WAV data was read in mono.
                        for (int i = 0; i < sr; i++)
                        {
                            int j = sr - i - 1;
                            buffer[2 * j] = buffer[2 * j + 1] = buffer[j];
                        }
                    }
                    buffer += sr * mpSample->mChannelCount;
                    mSamplesRead += sr;
                    mpSample->mSampleLoadCount = mSamplesRead;
                    mSamplesRemaining -= sr;
                    samplesRemaining -= sr;
                    if (mpSample->mSampleCount < SamplesPerBlock)
                    {
                        mPercentDone = 100.0f;
                    }
                    else
                    {
                        float blocksRead = mSamplesRead / SamplesPerBlock + 1;
                        mPercentDone = B * blocksRead;
                    }
                }
            }
        }
        return STATUS_FINISHED;
    }

} /* namespace od */
