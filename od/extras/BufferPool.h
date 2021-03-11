#pragma once

#include <hal/constants.h>
#include <hal/heap.h>
#include <string.h>
#include <vector>
#include <stdint.h>

namespace od
{

  template <typename T>
  class BufferPool
  {
  public:
    BufferPool() {}
    BufferPool(uint32_t bufferSize, uint32_t bufferCount)
    {
      allocate(bufferSize, bufferCount);
    }
    ~BufferPool()
    {
      deallocate();
    }

    bool allocate(uint32_t bufferSize, uint32_t bufferCount)
    {
      deallocate();
      uint32_t poolSizeInBytes = bufferCount * bufferSize * sizeof(T);
      mPool = (T *)Heap_memalign(CACHELINE_SIZE_MAX, poolSizeInBytes);
      if (mPool)
      {
        bzero(mPool, poolSizeInBytes);
        mBufferSize = bufferSize;
        mPoolSizeInBytes = poolSizeInBytes;
        mPoolSizeInBuffers = bufferCount;
        mFreeBuffers.reserve(bufferCount);
        // push buffers onto the free buffer stack in reverse order
        for (T *buffer = mPool + bufferCount * bufferSize - mBufferSize;
             buffer >= mPool; buffer -= mBufferSize)
        {
          mFreeBuffers.push_back(buffer);
        }
        return true;
      }
      else
      {
        return false;
      }
    }

    void deallocate()
    {
      if (mPool)
      {
        mFreeBuffers.clear();
        Heap_free(mPool);
        mPool = 0;
        mBufferSize = 0;
        mPoolSizeInBytes = 0;
        mPoolSizeInBuffers = 0;
      }
    }

    T *get()
    {
      if (mFreeBuffers.size() > 0)
      {
        T *buffer = mFreeBuffers.back();
        mFreeBuffers.pop_back();
        return buffer;
      }
      else
      {
        return 0;
      }
    }

    void release(T *buffer)
    {
      mFreeBuffers.push_back(buffer);
    }

    int countBuffersFree()
    {
      return (int)mFreeBuffers.size();
    }

    int countBuffersInUse()
    {
      return (int)(mPoolSizeInBuffers - mFreeBuffers.size());
    }

    float percentRemaining()
    {
      return mFreeBuffers.size() / (float)mPoolSizeInBuffers;
    }

    float percentUsed()
    {
      return 1.0f - percentRemaining();
    }

    bool owns(T *buffer)
    {
      uint8_t *p = (uint8_t *)buffer;
      uint8_t *start = (uint8_t *)mPool;
      return p >= start && p < start + mPoolSizeInBytes;
    }

    uint32_t mBufferSize = 0;
    uint32_t mPoolSizeInBytes = 0;
    uint32_t mPoolSizeInBuffers = 0;

  protected:
    T *mPool = 0;
    std::vector<T *> mFreeBuffers;
  };

  template <typename T>
  class WatermarkedBufferPool : public BufferPool<T>
  {
  public:
    T *get()
    {
      T *buffer = BufferPool<T>::get();
      if (buffer)
      {
        if (mMaxBuffersUsed < BufferPool<T>::countBuffersInUse())
        {
          mMaxBuffersUsed = BufferPool<T>::countBuffersInUse();
        }
        return buffer;
      }
      else
      {
        return 0;
      }
    }

    float percentMaximumUsed()
    {
      return 1.0f - mMaxBuffersUsed / (float)BufferPool<T>::mPoolSizeInBuffers;
    }

    int mMaxBuffersUsed = 0;
  };

} /* namespace od */
