#pragma once

#include <od/extras/ReferenceCounted.h>
#include <od/audio/WavFileWriter.h>
#include <od/audio/SampleLoadInfo.h>

namespace od
{

  class Sample : public ReferenceCounted
  {
  public:
    Sample();
    Sample(float sampleRate, uint32_t Nc, uint32_t Ns);
    ~Sample();

    bool allocateBuffer(uint32_t Nc, uint32_t Ns);
    int prepareForLoading(SampleLoadInfo &info);
    void setSampleRate(float rate);
    void zero();

    // editing
    void normalize(int from, int to, float targetAmplitude = 0.9f);
    void silence(int from, int to);
    void fadeIn(int from, int to);
    void fadeOut(int from, int to);
    void removeDC(int from, int to);
    void copyFrom(int from, int to, Sample *sample, int sourceStart);

    // call this after allocateBuffer() if not loading data from a file.
    void setMemoryOnly()
    {
      mSampleLoadCount = mSampleCount;
      mDirty = true;
    }

    int getSizeInBytes()
    {
      return mChannelCount * mSampleCount * sizeof(float);
    }

#ifndef SWIGLUA
    bool loadWavFile(const char *filename, bool verbose = false);
    bool saveWavFile(const char *filename, WavFileEncoding encoding);

    void setBuffer(float *buffer, uint32_t Nc, uint32_t Ns);
    void freeBuffer();

    inline float get(int i, int channel)
    {
      return mpData[i * mChannelCount + channel];
    }

    inline float getSafe(int i, int channel)
    {
      if (i >= 0 && i < (int)mSampleCount)
      {
        return mpData[i * mChannelCount + channel];
      }
      else
      {
        return 0.0f;
      }
    }

    inline void set(int i, int channel, float value)
    {
      mpData[i * mChannelCount + channel] = value;
    }

    inline float getMonoFromStereo(int i)
    {
      float *tmp = mpData + i * 2;
      return 0.5f * (tmp[0] + tmp[1]);
    }

    inline float getMonoFromMono(int i)
    {
      return mpData[i];
    }

    inline float calculateDurationRobustly(int Ns)
    {
      return (float)((double)Ns * (double)mSamplePeriod);
    }

    // sample buffer
    float *mpData = 0;

    // allows change detection
    uint32_t mWaterMark = 0;
    void alterWaterMark();
#endif

    // attributes
    uint32_t mChannelCount = 0;
    uint32_t mSampleCount = 0;
    uint32_t mSampleLoadCount = 0;
    float mSampleRate = 48000.0f;
    float mSamplePeriod = 1.0 / 48000.0f;
    int mOriginalBitDepth = 32;

    float mTotalSeconds = 0;
    // MM:SS.XXX
    int mMinutes = 0;
    int mSeconds = 0;
    int mMilliseconds = 0;

    bool mDirty = false;

    void setDirty()
    {
      mDirty = true;
    }

    void setClean()
    {
      mDirty = false;
    }
  };

} /* namespace od */
