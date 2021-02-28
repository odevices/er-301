#pragma once

#include <od/extras/BufferPool.h>

namespace od
{

  template <typename T>
  class MultiBufferPool
  {
  public:
    MultiBufferPool() {}
    ~MultiBufferPool()
    {
      for (BufferPool<T> *pool : mBufferPools)
      {
        delete pool;
      }
      mBufferPools.clear();
    }

    void add(uint32_t bufferSize, uint32_t bufferCount)
    {
      BufferPool<T> * pBufferPool = new BufferPool<T>(bufferSize, bufferCount);
      mBufferPools.push_back(pBufferPool);
    }

    uint32_t getActualSize(T *ptr)
    {
      for (BufferPool<T> *pool : mBufferPools)
      {
        if (pool->owns(ptr))
        {
          return pool->mBufferSize;
        }
      }
      return 0;
    }

    T *shrink(T *ptr, uint32_t n)
    {
      for (BufferPool<T> *pool : mBufferPools)
      {
        if (pool->owns(ptr))
        {
          // Already smallest possible.
          return ptr;
        }
        if (n <= pool->mBufferSize)
        {
          // Found a pool of smaller buffers that fits.
          T *buffer = pool->get();
          if (buffer)
          {
            memcpy(buffer, ptr, n);
            release(ptr);
            return buffer;
          }
        }
      }
      return ptr;
    }

    T *get(uint32_t n)
    {
      for (BufferPool<T> *pool : mBufferPools)
      {
        if (n <= pool->mBufferSize)
        {
          T *buffer = pool->get();
          if (buffer)
          {
            return buffer;
          }
        }
      }

      return 0;
    }

    bool release(T *ptr)
    {
      for (BufferPool<T> *pool : mBufferPools)
      {
        if (pool->owns(ptr))
        {
          pool->release(ptr);
          return true;
        }
      }
      return false;
    }

  private:
    // Assumed to be sorted in ascending buffer size.
    std::vector<BufferPool<T> *> mBufferPools;
  };

} // namespace od