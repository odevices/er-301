#pragma once

#include <hal/priorities.h>
#include <hal/concurrency/EventFlags.h>
#include <string>

namespace od
{

  class Thread
  {
  public:
    Thread(const char *name);
    Thread(const char *name, int priority);
    virtual ~Thread();

    void start();
    void join();
    void stop();
    bool running();

    static void sleep(uint32_t timeout);
    static void yield();

  protected:
    void *mThreadHandle = 0;
    std::string mName;
    bool mThreadRunning = false;

    EventFlags mEvents;
    static const uint32_t onThreadQuit = EventFlags::flag00;

    int mPriority = TASK_PRIORITY_APPTHREAD;

    friend struct ThreadRunner;
    virtual void run() = 0;
  };

} /* namespace od */
