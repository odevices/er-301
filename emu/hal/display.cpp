#include <hal/display.h>
#include <hal/log.h>
#include <emu/emu.h>
#include <stdbool.h>

typedef struct
{
  DisplayBuffer *lastPutBuffer;
  bool started;
} DisplayLocals;

static DisplayLocals local;

extern "C"
{

  void Display_init(void)
  {
    local.started = false;
    local.lastPutBuffer = 0;
  }

  void Display_deinit()
  {
    Display_stop();
  }

  void Display_start(void)
  {
    local.started = true;
  }

  void Display_stop(void)
  {
    local.started = false;
  }

  DisplayBuffer *Display_getBuffer()
  {
    if (local.started)
    {
      return emu::getDisplayBuffer();
    }
    return 0;
  }

  void Display_putBuffer(DisplayBuffer *buffer)
  {
    local.lastPutBuffer = buffer;
    emu::putDisplayBuffer(buffer);
  }

  DisplayBuffer *Display_getLastPutBuffer()
  {
    return local.lastPutBuffer;
  }

}