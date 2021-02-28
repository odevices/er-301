/*
 * Slices.h
 *
 *  Created on: 4 Jul 2016
 *      Author: clarkson
 */

#ifndef APP_AUDIO_SLICES_H_
#define APP_AUDIO_SLICES_H_

#include <od/extras/ReferenceCounted.h>
#include <od/extras/Lockable.h>
#include <od/audio/Slice.h>
#include <od/audio/SampleLoadInfo.h>
#include <vector>

namespace od
{

    typedef std::vector<Slice>::iterator SlicesIterator;

    class Slices : public ReferenceCounted, public Lockable
    {
    public:
        Slices();
        ~Slices();

        bool save(const char *filename);
        bool load(const char *filename);

        void setSampleRate(float rate);
        void copyFrom(int from, int to, Slices *slices, int sourceStart);
        void initFromLoadInfo(SampleLoadInfo &info);

        int size();
        bool isTrivial(int sampleCount);

#ifndef SWIGLUA

        SlicesIterator begin();
        SlicesIterator end();
        SlicesIterator getPositionAfter(int sample);
        SlicesIterator getPositionBefore(int sample);

        bool isIndexValid(int index);
        Slice *get(int index);
        int getPosition(int index);
        float getPositionInSeconds(int index);

        SlicesIterator findNearestNoCheck(int sample);
        SlicesIterator findNearest(int sample);

        SlicesIterator insert(const Slice &slice);

        void removeIterator(SlicesIterator pos);
        void removeRangeOfValues(int first, int last);
        void removeRangeOfIterators(SlicesIterator first, SlicesIterator last);

        void clear();
        void append(const Slice &slice);
        void sort();

        int getIntervalCount();
        int getIntervalLength(int index);
        int getIntervalStart(int index);

#endif

    private:
        // Slices are sorted in increasing slice.mStart order.
        std::vector<Slice> mSorted;
        float mSamplePeriod;

        // Intervals
        std::vector<int> mIntervalStarts;
        std::vector<int> mIntervalLengths;
        bool mIntervalsNeedRefresh = false;
        void refreshIntervals();
    };

} /* namespace od */

#endif /* APP_AUDIO_SLICES_H_ */
