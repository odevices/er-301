#pragma once

#include <hal/display.h>

namespace emu
{
  // Thread-safe.
  void putDisplayBuffer(DisplayBuffer *buffer);
  DisplayBuffer *getDisplayBuffer();
  int getEncoderValue();

  // Only call on main thread.
  int run(const char * xRoot, const char * rearRoot, const char * frontRoot);
}