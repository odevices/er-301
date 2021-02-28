#include <emu/emu.h>
#include <emu/Emulator.h>

namespace emu {
  static Emulator emulator;

  int run(const char * xRoot, const char * rearRoot, const char * frontRoot)
  {
    return emulator.run(xRoot, rearRoot, frontRoot);
  }

  DisplayBuffer *getDisplayBuffer()
  {
    return emulator.getDisplayBuffer();
  }

  void putDisplayBuffer(DisplayBuffer *buffer)
  {
    emulator.putDisplayBuffer(buffer);
  }

  int getEncoderValue()
  {
    return emulator.getEncoderValue();
  }
}