#pragma once

#include <od/tasks/Task.h>
#include <hal/concurrency/Mutex.h>
#include <vector>
#include <sstream>
#include <algorithm>

namespace od
{

  template <typename T>
  class ObjectCache : public Task
  {
  public:
    ObjectCache(const std::string &name, int watermark = 16, int capacity = 32) : Task(name), mWatermark(watermark), mCapacity(capacity)
    {
    }

    virtual ~ObjectCache()
    {
      mMutex.enter();
      for (T *object : mFree)
      {
        object->release();
      }
      for (T *object : mActive)
      {
        object->release();
      }
      mMutex.leave();
    }

    virtual void process(float *inputs, float *outputs)
    {
      mMutex.enter();
      for (T *object : mActive)
      {
        object->process();
      }
      mMutex.leave();
    }

    T *get()
    {
      T *object = 0;
      mMutex.enter();
      if (mFree.size() == 0)
      {
        if (totalSize() + 1 < mCapacity)
        {
          std::ostringstream ss;
          ss << mName << mActive.size();
          object = new T();
          object->setName(ss.str());
          object->attach();
        }
      }
      else
      {
        object = mFree.back();
        mFree.pop_back();
      }

      if (object)
      {
        mActive.push_back(object);
        object->reset();
      }
      mMutex.leave();
      return object;
    }

    void release(T *object)
    {
      mMutex.enter();
      typename std::vector<T *>::iterator i = std::find(mActive.begin(),
                                                        mActive.end(), object);
      if (i != mActive.end())
      {
        mActive.erase(i);
        if (totalSize() < mWatermark)
        {
          // keep the object
          mFree.push_back(object);
        }
        else
        {
          object->release();
        }
      }
      mMutex.leave();
    }

    void setWatermark(int n)
    {
      mWatermark = n;
    }

    int totalSize()
    {
      return (int)(mFree.size() + mActive.size());
    }

    int activeSize()
    {
      return (int)mActive.size();
    }

    int freeSize()
    {
      return (int)mFree.size();
    }

  private:
    Mutex mMutex;
    std::vector<T *> mFree;
    std::vector<T *> mActive;
    int mWatermark; // above this level we don't keep objects around
    int mCapacity;
  };

} // namespace od
