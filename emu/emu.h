#pragma once

#include <hal/display.h>

namespace emu
{
  // Thread-safe.
  void putDisplayBuffer(DisplayBuffer *buffer);
  DisplayBuffer *getDisplayBuffer();
  int getEncoderValue();
}