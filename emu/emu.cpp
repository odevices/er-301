#include <emu/emu.h>
#include <emu/Emulator.h>

namespace emu
{
  static Emulator emulator;

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

  bool isRearCardPresent()
  {
    return emulator.isRearCardPresent();
  }

  bool isFrontCardPresent()
  {
    return emulator.isFrontCardPresent();
  }
}

int main(int argc, char **argv)
{
  return emu::emulator.run(argc, argv);
}
