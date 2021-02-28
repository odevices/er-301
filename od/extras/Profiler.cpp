#include <od/extras/Profiler.h>
#include <hal/log.h>

namespace od
{

  Profiler &Profiler::singleton()
  {
    static Profiler p;
    return p;
  }

  Profiler::Profiler()
  {
  }

  void Profiler::start()
  {
    Profiler &p = singleton();
    p.mStart = ticks();
    for (ExecutionTimer *timer : p.mTimers)
    {
      timer->mFreeze = false;
      timer->reset();
    }
  }

  void Profiler::stop()
  {
    Profiler &p = singleton();
    p.mElapsed = ticks() - p.mStart;
    for (ExecutionTimer *timer : p.mTimers)
    {
      timer->mFreeze = true;
    }
  }

  void Profiler::print()
  {
    Profiler &p = singleton();

    if (p.mTimers.size() == 0)
    {
      logInfo("Profiler: No execution timers are registered.");
      return;
    }

    float secs = ticks2secs(p.mElapsed);
    float cpu;
    uint32_t cycles_per_call, total_elapsed = 0;
    uint32_t calls_per_sec;

    for (ExecutionTimer *timer : p.mTimers)
    {
      if (secs > 0.0f && timer->mCount > 0)
      {
        cpu = ticks2secs(timer->mElapsed) / secs;
        cycles_per_call = timer->mElapsed / timer->mCount;
        calls_per_sec = timer->mCount / secs;
        if (timer->mIncludeInTotal)
        {
          total_elapsed += timer->mElapsed;
        }

        logInfo("%s: %f%% (%d ticks, %d Hz)",
                timer->mName, 100.0f * cpu, cycles_per_call, calls_per_sec);
      }
      else
      {
        logInfo("[%s]: no data", timer->mName);
      }
    }

    if (secs > 0.0f)
    {
      cpu = ticks2secs(total_elapsed) / secs;
      logInfo("total: %f%%", 100.0f * cpu);
    }
  }

  void Profiler::add(ExecutionTimer *timer, const char *name, bool includeInTotal)
  {
    if (!timer->mRegistered)
    {
      timer->mName = name;
      timer->mIncludeInTotal = includeInTotal;
      timer->reset();
      timer->mFreeze = false;
      timer->mRegistered = true;
      Profiler::singleton().mTimers.push_back(timer);
    }
  }

  void Profiler::remove(ExecutionTimer *timer)
  {
    if (timer->mRegistered)
    {
      Profiler::singleton().mTimers.remove(timer);
      timer->mRegistered = false;
    }
  }

  ExecutionTimer::ExecutionTimer()
  {
  }

  ExecutionTimer::~ExecutionTimer()
  {
    if (mRegistered)
    {
      Profiler::remove(this);
      mRegistered = false;
    }
  }

  void ExecutionTimer::start()
  {
    mStart = ticks();
  }

  void ExecutionTimer::stop()
  {
    if (mFreeze)
      return;
    mElapsed += ticks() - mStart;
    mCount++;
  }

  void ExecutionTimer::pause()
  {
    if (mFreeze)
      return;
    mElapsed += ticks() - mStart;
  }

  void ExecutionTimer::unpause()
  {
    mStart = ticks();
  }

  void ExecutionTimer::reset()
  {
    mElapsed = 0;
    mCount = 0;
  }

} // namespace od