#pragma once

#include <hal/timing.h>
#include <list>

namespace od
{

  class ExecutionTimer;
  class Profiler
  {
  public:
    static void start();
    static void stop();
    static void print();
#ifndef SWIGLUA
    static void add(ExecutionTimer *timer, const char *name, bool includeInTotal = true);
    static void remove(ExecutionTimer *timer);
#endif

  private:
    Profiler();
    static Profiler &singleton();

    std::list<ExecutionTimer *> mTimers;
    tick_t mStart = 0;
    tick_t mElapsed = 0;
  };

#ifndef SWIGLUA
  class ExecutionTimer
  {
  public:
    ExecutionTimer();
    ~ExecutionTimer();

    void start();
    void stop();
    void pause();
    void unpause();
    void reset();

    tick_t mStart = 0;
    tick_t mElapsed = 0;
    uint32_t mCount = 0;
    const char *mName;
    bool mIncludeInTotal;
    bool mFreeze = true;

  private:
    friend Profiler;
    bool mRegistered = false;
  };
#endif

} // namespace od