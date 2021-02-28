#pragma once

#include <hal/concurrency/Mutex.h>
#include <od/audio/Sample.h>
#include <atomic>

namespace od
{

  class SampleFifo
  {
  public:
    SampleFifo();
    virtual ~SampleFifo();

    void allocateBuffer(int Nc, int Ns);
    void setSampleRate(float rate)
    {
      mBuffer.setSampleRate(rate);
    }
    void freeBuffer();
    void clear();
    void zero();
    void zeroAndFill();

    void pushMono(float *buffer, int n);
    void pushStereo(float *buffer, int n);
    void pop(int n);
    void popAll();

    inline float *front()
    {
      return mBuffer.mpData + mHead * mChannelCount;
    }

    inline float *recent(int n)
    {
      return mBuffer.mpData + (mTail + mCapacity - n) * mChannelCount;
    }

    inline int offsetToRecent(int n)
    {
      return mTail + mCapacity - n;
    }

    inline Sample *getSample()
    {
      return &mBuffer;
    }

    inline int size()
    {
      return mSize;
    }

    inline int capacity()
    {
      return mCapacity;
    }

    inline int free()
    {
      return mCapacity - mSize;
    }

    inline int getChannelCount()
    {
      return mChannelCount;
    }

    inline bool canPush(int n)
    {
      return mCapacity - mSize >= n;
    }

  private:
    int mCapacity = 0;
    int mChannelCount = 1;
    Sample mBuffer;
    std::atomic<int> mHead{0};
    std::atomic<int> mTail{0};
    std::atomic<int> mSize{0};

    inline float *back()
    {
      return mBuffer.mpData + mTail * mChannelCount;
    }

    inline float *back2()
    {
      return mBuffer.mpData + (mTail + mCapacity) * mChannelCount;
    }

    inline void setTail(int n)
    {
      mTail = (n < mCapacity) ? n : 0;
    }

    inline void copyMonoToMono(float *dst, float *src, int n);
    inline void copyStereoToStereo(float *dst, float *src, int n);
    inline void copyStereoToMono(float *dst, float *src, int n);
    inline void copyMonoToStereo(float *dst, float *src, int n);
  };

} /* namespace od */
