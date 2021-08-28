#include <od/ui/Busy.h>
#include <od/UIThread.h>
#include <hal/pwm.h>
#include <hal/events.h>
#include <hal/log.h>

namespace od
{

  Busy::Busy() : Thread("busy", TASK_PRIORITY_BUSY)
  {
  }

  Busy::~Busy()
  {
  }

  void Busy::enable()
  {
    if (mEnabled)
      return;
    mEvents.post(onEnable);
    while (running() && !mEnabled)
    {
      Thread::sleep(10);
    }
    logDebug(10, "BUSY");
  }

  void Busy::disable()
  {
    if (!mEnabled)
      return;
    mEvents.post(onDisable);
    while (running() && mEnabled)
    {
      Thread::sleep(10);
    }
    logDebug(10, "IDLE");
  }

  void Busy::run()
  {
    int position = 0;
    int count = 0;
    int step = 1;

    while (1)
    {
      bool hadQuitEvent = false;
      bool hadModeEvent = false;
      bool hadStorageEvent = false;
      bool hadShiftReleaseEvent = false;
      if (mEvents.waitForAny(onThreadQuit | onEnable | onDisable) & onThreadQuit)
      {
        return;
      }

      mEnabled = true;
      while (!hadQuitEvent)
      {
        if (Events_waitWithTimeout(10))
        {
          UIThread::restartScreenSaverTimer();
          while (1)
          {
            uint32_t e = Events_pull();
            if (e == EVENT_NONE)
            {
              break;
            }
            if (e == EVENT_DISPLAY_READY)
            {
              UIThread::updateDisplay();
              count++;
              if (count == 5)
              {
                position += step;
                count = 0;
              }

              if (position == 5)
              {
                step = -1;
              }
              else if (position == 0)
              {
                step = 1;
              }

              switch (position)
              {
              case 0:
                Pwm_set_raw(8, 1.0f, 1.0f); // A3
                break;
              case 1:
                Pwm_set_raw(4, 1.0f, 1.0f); // A2
                Pwm_set_raw(9, 1.0f, 1.0f); // B3
                break;
              case 2:
                Pwm_set_raw(0, 1.0f, 1.0f);  // A1
                Pwm_set_raw(5, 1.0f, 1.0f);  // B2
                Pwm_set_raw(10, 1.0f, 1.0f); // C3
                break;
              case 3:
                Pwm_set_raw(1, 1.0f, 1.0f);  // B1
                Pwm_set_raw(6, 1.0f, 1.0f);  // C2
                Pwm_set_raw(11, 1.0f, 1.0f); // D3
                break;
              case 4:
                Pwm_set_raw(2, 1.0f, 1.0f); // C1
                Pwm_set_raw(7, 1.0f, 1.0f); // D2
                break;
              case 5:
                Pwm_set_raw(3, 1.0f, 1.0f); // D1
                break;
              }
            }
            else if (e == EVENT_MODE)
            {
              hadModeEvent = true;
            }
            else if (e == EVENT_STORAGE)
            {
              hadStorageEvent = true;
            }
            else if (e == EVENT_RELEASE_SHIFT)
            {
              hadShiftReleaseEvent = true;
            }
            else if (e == EVENT_QUIT)
            {
              hadQuitEvent = true;
            }
          }
        }
        uint32_t mask = mEvents.getPosted();
        if (mask)
        {
          mEvents.clear();
          if (mask & onThreadQuit)
            return;
          if (mask & onDisable)
          {
            break;
          }
        }
      }
      mEnabled = false;
      // re-notify if there was an ignored event on the toggle switches
      if (hadModeEvent)
      {
        Events_push(EVENT_MODE);
        hadModeEvent = false;
      }
      if (hadStorageEvent)
      {
        Events_push(EVENT_STORAGE);
        hadStorageEvent = false;
      }
      if (hadShiftReleaseEvent)
      {
        Events_push(EVENT_RELEASE_SHIFT);
        hadShiftReleaseEvent = false;
      }
      if (hadQuitEvent)
      {
        Events_push(EVENT_QUIT);
        break;
      }
    }
    // should never get here
    mEnabled = false;
  }

} /* namespace od */
