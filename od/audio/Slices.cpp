/*
 * Slices.cpp
 *
 *  Created on: 4 Jul 2016
 *      Author: clarkson
 */

#include <od/audio/Slices.h>
#include <od/extras/FileWriter.h>
#include <od/extras/FileReader.h>
#include <hal/simd.h>
#include <od/config.h>
#include <hal/log.h>
#include <algorithm>

namespace od
{

  Slices::Slices()
  {
    mSamplePeriod = globalConfig.samplePeriod;
  }

  Slices::~Slices()
  {
  }

  bool Slices::save(const char *filename)
  {
    FileWriter writer;
    int n = mSorted.size();

    if (!writer.open(filename))
    {
      return false;
    }

    writer.writeHeader("Slices", 0, 0);
    writer.writeBytes(&n, sizeof(int));
    for (Slice &slice : mSorted)
    {
      writer.writeBytes(&slice.mStart, sizeof(int));
      writer.writeBytes(&slice.mLoopStart, sizeof(int));
      writer.writeBytes(&slice.mLoopEnd, sizeof(int));
      writer.writeBytes(&slice.mSpeed, sizeof(float));
    }

    writer.close();
    return true;
  }

  bool Slices::load(const char *filename)
  {
    FileReader reader;
    std::string description;
    uint32_t major, minor;
    int n = 0;

    if (!reader.open(filename))
    {
      logWarn("Failed to open %s", filename);
      return false;
    }

    if (!reader.readHeader(description, major, minor))
    {
      logWarn("Failed to read header.");
      return false;
    }

    if (reader.readBytes(&n, sizeof(int)) != sizeof(int))
    {
      logWarn("Failed to read # of slices.");
      return false;
    }

    if (n == 0)
    {
      return true;
    }

    mSorted.clear();
    mIntervalsNeedRefresh = true;
    mSorted.reserve(n);
    for (int i = 0; i < n; i++)
    {
      Slice slice;
      if (reader.readBytes(&slice.mStart, sizeof(int)) != sizeof(int))
      {
        logWarn("Failed to read slice %d start.", i);
        return false;
      }
      if (reader.readBytes(&slice.mLoopStart, sizeof(int)) != sizeof(int))
      {
        logWarn("Failed to read slice %d loop start.", i);
        return false;
      }
      if (reader.readBytes(&slice.mLoopEnd, sizeof(int)) != sizeof(int))
      {
        logWarn("Failed to read slice %d loop end.", i);
        return false;
      }
      if (reader.readBytes(&slice.mSpeed, sizeof(float)) != sizeof(float))
      {
        logWarn("Failed to read slice %d speed.", i);
        return false;
      }

      mSorted.push_back(slice);
    }

    sort();

    return true;
  }

  void Slices::initFromLoadInfo(SampleLoadInfo &info)
  {
    clear();
    uint32_t pos = 0;
    for (SampleLoadInfo::Entry &entry : info.mEntries)
    {
      insert(pos);
      pos += entry.sampleCount;
    }
    insert(info.mSampleCount);
    mIntervalsNeedRefresh = true;
  }

  SlicesIterator Slices::getPositionAfter(int sample)
  {
    return std::lower_bound(mSorted.begin(), mSorted.end(), sample);
  }

  SlicesIterator Slices::getPositionBefore(int sample)
  {
    SlicesIterator i = std::lower_bound(mSorted.begin(), mSorted.end(), sample);
    if (std::distance(mSorted.begin(), i) > 0)
    {
      i--;
    }
    return i;
  }

  SlicesIterator Slices::findNearestNoCheck(int sample)
  {
    SlicesIterator i = std::lower_bound(mSorted.begin(), mSorted.end(), sample);
    if (i == mSorted.begin())
    {
      // sample < all slices
      return i;
    }
    else if (i == mSorted.end())
    {
      // sample > all slices
      return i - 1;
    }
    else
    {
      SlicesIterator j = i;
      j--;
      if ((sample - (*j).mStart) < ((*i).mStart - sample))
      {
        return j;
      }
      else
      {
        return i;
      }
    }
  }

  SlicesIterator Slices::insert(const Slice &slice)
  {
    SlicesIterator i = std::lower_bound(mSorted.begin(), mSorted.end(), slice);
    i = mSorted.insert(i, slice);
    mIntervalsNeedRefresh = true;
    // Check the loop of the previous slice doesn't overlap with this new slice.
    if (i > mSorted.begin())
    {
      i--;
      Slice &prevSlice = *i;
      if (prevSlice.mLoopStart > slice.mStart)
      {
        prevSlice.mLoopStart = 0;
        prevSlice.mLoopEnd = 0;
      }
      else if (prevSlice.mLoopEnd > slice.mStart)
      {
        prevSlice.mLoopEnd = slice.mStart;
      }
    }

    return i;
  }

  void Slices::removeRangeOfValues(int first, int last)
  {
    SlicesIterator i = std::lower_bound(mSorted.begin(), mSorted.end(), first);
    SlicesIterator j = std::upper_bound(mSorted.begin(), mSorted.end(), last);
    mSorted.erase(i, j);
    mIntervalsNeedRefresh = true;
  }

  void Slices::removeIterator(SlicesIterator i)
  {
    mSorted.erase(i);
    mIntervalsNeedRefresh = true;
  }

  void Slices::removeRangeOfIterators(SlicesIterator first, SlicesIterator last)
  {
    mSorted.erase(first, last);
    mIntervalsNeedRefresh = true;
  }

  void Slices::append(const Slice &slice)
  {
    mSorted.push_back(slice);
    mIntervalsNeedRefresh = true;
  }

  void Slices::sort()
  {
    std::sort(mSorted.begin(), mSorted.end());
  }

  float Slices::getPositionInSeconds(int index)
  {
    Slice *slice = get(index);
    if (slice)
    {
      return ((double)slice->mStart) * mSamplePeriod;
    }
    else
    {
      return 0.0f;
    }
  }

  // Starting at sourceStart, copy slices from 'srcSlices' to the interval [from, to].
  void Slices::copyFrom(int from, int to, Slices *srcSlices, int sourceStart)
  {
    for (Slice &slice : srcSlices->mSorted)
    {
      if ((int)slice.mStart < sourceStart)
      {
        continue;
      }
      int offset = from - sourceStart;
      int pos = slice.mStart + offset;
      if (pos < to)
      {
        SlicesIterator i = insert(pos);
        Slice &newSlice = *i;
        if (slice.mLoopStart > 0)
        {
          newSlice.mLoopStart = slice.mLoopStart + offset;
        }
        if (slice.mLoopEnd > 0)
        {
          newSlice.mLoopEnd = slice.mLoopEnd + offset;
        }
        newSlice.mSpeed = slice.mSpeed;
      }
      else
      {
        break;
      }
    }
  }

  SlicesIterator Slices::begin()
  {
    return mSorted.begin();
  }

  SlicesIterator Slices::end()
  {
    return mSorted.end();
  }

  bool Slices::isIndexValid(int index)
  {
    return index >= 0 && index < (int)mSorted.size();
  }

  Slice *Slices::get(int index)
  {
    int n = size();
    if (n == 0)
    {
      return 0;
    }
    if (index < 0)
    {
      index = 0;
    }
    if (index >= n)
    {
      index = n - 1;
    }
    return &mSorted.at(index);
  }

  int Slices::getPosition(int index)
  {
    Slice *slice = get(index);
    if (slice)
    {
      return slice->mStart;
    }
    return 0;
  }

  SlicesIterator Slices::findNearest(int sample)
  {
    if (mSorted.size() == 0)
    {
      return mSorted.end();
    }
    else
    {
      return findNearestNoCheck(sample);
    }
  }

  void Slices::clear()
  {
    mSorted.clear();
    mIntervalsNeedRefresh = true;
  }

  int Slices::size()
  {
    return mSorted.size();
  }

  void Slices::setSampleRate(float rate)
  {
    if (rate > 0.001f)
    {
      mSamplePeriod = 1.0f / rate;
    }
  }

  void Slices::refreshIntervals()
  {
    mIntervalStarts.clear();
    mIntervalLengths.clear();
    if (mSorted.size() < 2)
    {
      mIntervalsNeedRefresh = false;
      return;
    }

    int n = size() - 1;
    for (int i = 0; i < n; i++)
    {
      Slice &s0 = mSorted.at(i);
      Slice &s1 = mSorted.at(i + 1);
      int L = s1.mStart - s0.mStart;
      if (L > 2)
      {
        mIntervalStarts.push_back(s0.mStart);
        mIntervalLengths.push_back(L);
      }
    }
    mIntervalsNeedRefresh = false;
  }

  int Slices::getIntervalCount()
  {
    if (mIntervalsNeedRefresh)
    {
      refreshIntervals();
    }
    return mIntervalLengths.size();
  }

  int Slices::getIntervalLength(int index)
  {
    if (mIntervalsNeedRefresh)
    {
      refreshIntervals();
    }
    return mIntervalLengths.at(index);
  }

  int Slices::getIntervalStart(int index)
  {
    if (mIntervalsNeedRefresh)
    {
      refreshIntervals();
    }
    return mIntervalStarts.at(index);
  }

  bool Slices::isTrivial(int sampleCount)
  {
    switch (size())
    {
    case 0:
      return true;
    case 1:
      return getPosition(0) == 0;
    case 2:
      return getPosition(0) == 0 && getPosition(1) == sampleCount;
    default:
      return false;
    }
  }

} /* namespace od */
