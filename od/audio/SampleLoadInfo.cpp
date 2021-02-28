#include <od/audio/SampleLoadInfo.h>
#include <od/audio/WavFileReader.h>
#include <hal/ops.h>

namespace od
{

    SampleLoadInfo::SampleLoadInfo()
    {
    }

    SampleLoadInfo::~SampleLoadInfo()
    {
    }

    bool SampleLoadInfo::add(const char *filename, bool verbose)
    {
        WavFileReader reader;

        if (!reader.open(filename))
        {
            return false;
        }

        if (verbose)
        {
            reader.describe();
        }

        reader.close();

        int sampleCount = mSampleCount + reader.getSampleCount();
        int channelCount = MAX(mChannelCount, (int)reader.getChannelCount());
        mSampleCount = sampleCount;
        mChannelCount = channelCount;
        mEntries.emplace_back(filename, reader.getSampleCount(),
                              reader.getChannelCount(), reader.getSampleRate(),
                              reader.getBitDepth());
        return true;
    }

    void SampleLoadInfo::clear()
    {
        mEntries.clear();
        mSampleCount = 0;
        mChannelCount = 0;
    }

    void SampleLoadInfo::reserve(int n)
    {
        mEntries.reserve(n);
    }

} /* namespace od */
