#pragma once

#include <od/extras/ReferenceCounted.h>
#include <vector>
#include <string>

namespace od
{

    class SampleLoadInfo : public ReferenceCounted
    {
    public:
        SampleLoadInfo();
        virtual ~SampleLoadInfo();

        void reserve(int n);
        bool add(const char *filename, bool verbose = false);
        void clear();
        int getCount()
        {
            return mEntries.size();
        }

#ifndef SWIGLUA
        int mSampleCount = 0;
        int mChannelCount = 0;

        struct Entry
        {
            Entry(const std::string &filename, int sampleCount, int channelCount,
                  float sampleRate, int bitDepth) : filename(filename), sampleCount(sampleCount), channelCount(channelCount), sampleRate(sampleRate), bitDepth(bitDepth)
            {
            }
            std::string filename;
            int sampleCount;
            int channelCount;
            float sampleRate;
            int bitDepth;
        };

        std::vector<Entry> mEntries;
#endif
    };

} /* namespace od */
