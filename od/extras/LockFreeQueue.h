#pragma once

#include <cstddef>
#include <atomic>

namespace od
{

  template <typename T, size_t N>
  class LockFreeQueue
  {
  public:
    LockFreeQueue() : front(0), back(0), depth(0)
    {
    }

    void clear()
    {
      front = 0;
      back = 0;
      pfront = 0;
      cback = 0;
      depth = 0;
    }

    bool push(const T &value)
    {
      size_t b;
      b = back.load(std::memory_order_relaxed);
      if (pfront + N == b)
      {
        pfront = front.load(std::memory_order_acquire);
        if (pfront + N == b)
        {
          return false;
        }
      }
      data[b % N] = value;
      back.store(b + 1, std::memory_order_release);
      return true;
    }

    bool pop(T *value)
    {
      size_t f;
      f = front.load(std::memory_order_relaxed);
      if (cback == f)
      {
        cback = back.load(std::memory_order_acquire);
        if (cback == f)
        {
          return false;
        }
      }
      *value = data[f % N];
      front.store(f + 1, std::memory_order_release);
      return true;
    }

    //  Alias for pop.
    bool pull(T *value)
    {
      return pop(value);
    }

    bool peek(T *value)
    {
      size_t f;
      f = front.load(std::memory_order_relaxed);
      if (cback == f)
      {
        cback = back.load(std::memory_order_acquire);
        if (cback == f)
        {
          return false;
        }
      }
      *value = data[f % N];
      return true;
    }

    bool empty()
    {
      size_t f;
      f = front.load(std::memory_order_relaxed);
      if (cback == f)
      {
        cback = back.load(std::memory_order_acquire);
        if (cback == f)
        {
          return true;
        }
      }
      return false;
    }

    int count()
    {
      pfront = front.load(std::memory_order_acquire);
      cback = back.load(std::memory_order_acquire);
      if (pfront < cback)
      {
        return cback - pfront;
      }
      else
      {
        return cback + (N - pfront);
      }
    }

  private:
    std::atomic<size_t> front;
    size_t pfront = 0;
    std::atomic<size_t> back;
    size_t cback = 0;
    std::atomic<size_t> depth;
    T data[N];
  };

} // namespace od
