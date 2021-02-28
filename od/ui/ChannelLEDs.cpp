#include <od/ui/ChannelLEDs.h>
#include <hal/gpio.h>
#include <hal/constants.h>

namespace od
{

  struct State
  {
    bool on = false;
    bool flashing = false;
  };

  static State mState[4];
  static int mUpdateCount = 0;
  static int mPhase = 0;

  void ChannelLEDs_update()
  {
    if (mUpdateCount < 3)
    {
      mUpdateCount++;
    }
    else
    {
      mUpdateCount = 0;
      mPhase++;
      if (mPhase == 8)
      {
        mPhase = 0;
      }

      for (int i = 0; i < 4; i++)
      {
        uint32_t id = LED_OUT1 + i;
        if (mState[i].on)
        {
          if (mState[i].flashing)
          {
            if (mPhase < 7)
            {
              Led_on(id);
            }
            else
            {
              Led_off(id);
            }
          }
          else
          {
            Led_on(id);
          }
        }
        else
        {
          if (mState[i].flashing)
          {
            if (mPhase % 2 == 0)
            {
              Led_on(id);
            }
            else
            {
              Led_off(id);
            }
          }
          else
          {
            Led_off(id);
          }
        }
      }
    }
  }

  void ChannelLEDs_on(int i)
  {
    mState[i].on = true;
  }

  void ChannelLEDs_off(int i)
  {
    mState[i].on = false;
  }

  void ChannelLEDs_flash(int i)
  {
    mState[i].flashing = true;
  }

  void ChannelLEDs_steady(int i)
  {
    mState[i].flashing = false;
  }

} // namespace od