#include <emu/tls.h>
#include <od/extras/ReferenceCounted.h>
#include <hal/concurrency/Thread.h>
//#define BUILDOPT_DEBUG_LEVEL 10
#include <hal/log.h>
#include <SDL2/SDL_thread.h>
#include <SDL2/SDL_timer.h>

#ifdef BUILDOPT_VERBOSE
#include <typeinfo>
#endif

namespace od
{

  struct ThreadRunner
  {
    static int threadEntry(void *ptr)
    {
      od::Thread *thread = (od::Thread *)ptr;
      logAssert(thread);
      TLS_setName(thread->mName.c_str());
      logInfo("Thread starting.");
      if (thread->mPriority < TASK_PRIORITY_REALTIME)
      {
        SDL_SetThreadPriority(SDL_THREAD_PRIORITY_NORMAL);
      }
      else
      {
        SDL_SetThreadPriority(SDL_THREAD_PRIORITY_NORMAL);
        // SDL_SetThreadPriority(SDL_THREAD_PRIORITY_TIME_CRITICAL);
      }
      thread->run();
      return 0;
    }
  };

  void Thread::sleep(uint32_t timeout)
  {
    SDL_Delay(timeout);
  }

  void Thread::yield()
  {
    SDL_Delay(0);
  }

  Thread::Thread(const char *name) : mName(name)
  {
    logDebug(1, "Thread(%s): constructor", name);
  }

  Thread::Thread(const char *name, int priority) : mName(name), mPriority(priority)
  {
    logDebug(1, "Thread(%s): constructor", name);
  }

  Thread::~Thread()
  {
    logDebug(1, "Thread(%s): destructor", mName.c_str());
    if (mThreadHandle)
    {
      stop();
      join();
    }
  }

  void Thread::start()
  {
#ifdef BUILDOPT_VERBOSE
    std::string classname = demangle(typeid(*this).name());
    logDebug(1, "%s(0x%x): start", classname.c_str(), this);
#endif
    mThreadHandle = (void *)SDL_CreateThread(ThreadRunner::threadEntry, mName.c_str(), (void *)this);
    if (mThreadHandle == 0)
    {
      logError("Failed to create SDL Thread.");
    }
  }

  bool Thread::started()
  {
    return mThreadHandle != 0;
  }

  void Thread::stop()
  {
#ifdef BUILDOPT_VERBOSE
    std::string classname = demangle(typeid(*this).name());
    logDebug(1, "%s(0x%x): requesting stop", classname.c_str(), this);
#endif
    mEvents.post(onThreadQuit);
  }

  void Thread::join()
  {
    if (mThreadHandle)
    {
      int ret;
      SDL_Thread *thread = (SDL_Thread *)mThreadHandle;
      SDL_WaitThread(thread, &ret);
#ifdef BUILDOPT_VERBOSE
      logDebug(1, "%s(0x%x): stopped", classname.c_str(), this);
#endif
    }
    else
    {
      logError("Trying to join a thread that has not started.");
    }
  }

} /* namespace od */
