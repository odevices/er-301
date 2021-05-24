#pragma once

namespace od
{

  class Mutex
  {
  public:
    Mutex();
    virtual ~Mutex();

    void enter();
    void leave();
    bool tryEnter();
    bool tryEnter(unsigned int timeout);

  private:
    void *mHandle;
  };

  class Lock
  {
  public:
    Lock(Mutex &mutex) : mMutex(mutex)
    {
      mMutex.enter();
    }

    ~Lock()
    {
      mMutex.leave();
    }

  private:
    Mutex &mMutex;
  };

} /* namespace od */
