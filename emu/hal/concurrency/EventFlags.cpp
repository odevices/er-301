#include <hal/concurrency/EventFlags.h>
#include <SDL2/SDL_mutex.h>

namespace od
{

  struct Pimp
  {
    Pimp()
    {
      mMutex = SDL_CreateMutex();
      mCond = SDL_CreateCond();
    }

    ~Pimp()
    {
      SDL_DestroyCond(mCond);
      SDL_DestroyMutex(mMutex);
    }

    uint32_t pend(uint32_t all, uint32_t any, uint32_t timeout)
    {
      uint32_t matching;
      SDL_LockMutex(mMutex);
      while (!(matching = check(all, any)))
      {
        if (SDL_CondWaitTimeout(mCond, mMutex, timeout) == SDL_MUTEX_TIMEDOUT)
        {
          break;
        }
      }
      SDL_UnlockMutex(mMutex);
      return matching;
    }

    void post(uint32_t flags)
    {
      SDL_LockMutex(mMutex);
      mPosted |= flags;
      SDL_CondSignal(mCond);
      SDL_UnlockMutex(mMutex);
    }

    uint32_t getPosted()
    {
      uint32_t posted;
      SDL_LockMutex(mMutex);
      posted = mPosted;
      SDL_UnlockMutex(mMutex);
      return posted;
    }

  private:
    SDL_mutex *mMutex;
    SDL_cond *mCond;
    uint32_t mPosted = 0;

    uint32_t check(uint32_t andMask, uint32_t orMask)
    {
      uint32_t matching;

      matching = orMask & mPosted;

      if ((andMask & mPosted) == andMask)
      {
        matching |= andMask;
      }

      if (matching)
      {
        /* consume ALL the matching events */
        mPosted &= ~matching;
      }

      return (matching);
    }
  };

  EventFlags::EventFlags()
  {
    mHandle = (void *)new Pimp();
  }

  EventFlags::~EventFlags()
  {
    delete (Pimp *)mHandle;
  }

  void EventFlags::clear(uint32_t flags)
  {
    Pimp *pimp = (Pimp *)mHandle;
    pimp->pend(0, flags, 0);
  }

  void EventFlags::post(uint32_t flags)
  {
    Pimp *pimp = (Pimp *)mHandle;
    pimp->post(flags);
  }

  uint32_t EventFlags::getPosted()
  {
    Pimp *pimp = (Pimp *)mHandle;
    return pimp->getPosted();
  }

  uint32_t EventFlags::waitForAny(uint32_t flags)
  {
    Pimp *pimp = (Pimp *)mHandle;
    return pimp->pend(0, flags, SDL_MUTEX_MAXWAIT);
  }

  uint32_t EventFlags::waitForAny(uint32_t flags, uint32_t timeout)
  {
    Pimp *pimp = (Pimp *)mHandle;
    return pimp->pend(0, flags, timeout);
  }

  uint32_t EventFlags::waitForAll(uint32_t flags)
  {
    Pimp *pimp = (Pimp *)mHandle;
    return pimp->pend(flags, 0, SDL_MUTEX_MAXWAIT);
  }

  uint32_t EventFlags::waitForAll(uint32_t flags, uint32_t timeout)
  {
    Pimp *pimp = (Pimp *)mHandle;
    return pimp->pend(flags, 0, timeout);
  }

  uint32_t EventFlags::wait(uint32_t allFlags, uint32_t anyFlags)
  {
    Pimp *pimp = (Pimp *)mHandle;
    return pimp->pend(allFlags, anyFlags, SDL_MUTEX_MAXWAIT);
  }

  uint32_t EventFlags::wait(uint32_t allFlags, uint32_t anyFlags, uint32_t timeout)
  {
    Pimp *pimp = (Pimp *)mHandle;
    return pimp->pend(allFlags, anyFlags, timeout);
  }

} // namespace od