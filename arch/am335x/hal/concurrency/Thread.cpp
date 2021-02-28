#include <hal/concurrency/Thread.h>
#include <hal/log.h>
#include <od/extras/ReferenceCounted.h>
#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/knl/Event.h>
#include <ti/sysbios/BIOS.h>

#ifdef BUILDOPT_VERBOSE
#include <typeinfo>
#endif

namespace od
{

  struct ThreadRunner
  {
    static Void taskEntry(UArg arg0, UArg arg1)
    {
      od::Thread *thread = (od::Thread *)arg0;
      logAssert(thread);
      logInfo("Thread starting: %s", thread->mName.c_str());
      thread->mThreadRunning = true;
      thread->run();
      thread->mThreadRunning = false;
    }
  };

  void Thread::sleep(uint32_t timeout)
  {
    Task_sleep(timeout);
  }

  void Thread::yield()
  {
    Task_yield();
  }

  Thread::Thread(const char *name) : mName(name)
  {
  }

  Thread::Thread(const char *name, int priority) : mName(name), mPriority(priority)
  {
  }

  Thread::~Thread()
  {
    if (mThreadHandle)
    {
      stop();
      join();
      if (Task_deleteTerminatedTasks)
      {
        // Will be deleted automatically by the Idle Task
      }
      else
      {
        Task_delete((Task_Handle *)&mThreadHandle);
      }
    }
  }

  void Thread::start()
  {
    Task_Params params;
    Task_Params_init(&params);
    params.priority = mPriority;
    params.stackSize = 4096;
    params.arg0 = (UArg)this;
    params.env = (Ptr)mName.c_str();
    mThreadHandle = (void *)Task_create(ThreadRunner::taskEntry, &params, NULL);
#ifdef BUILDOPT_VERBOSE
    std::string classname = demangle(typeid(*this).name());
    logDebug(1, "%s(0x%x): start", classname.c_str(), this);
#endif
  }

  bool Thread::running()
  {
    return mThreadRunning;
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
    // set the thread priority to equal the calling one
    Task_setPri((Task_Handle)mThreadHandle, Task_getPri(Task_self()));
    while (Task_getMode((Task_Handle)mThreadHandle) != Task_Mode_TERMINATED)
    {
      Task_yield();
    }
#ifdef BUILDOPT_VERBOSE
    logDebug(1, "%s(0x%x): stopped", classname.c_str(), this);
#endif
  }

} /* namespace od */
