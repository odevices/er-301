#include <hal/log.h>
#include <hal/encoder.h>
#include <emu/emu.h>

extern "C"
{

  static int lastValue = 0;

  void Encoder_init(void)
  {
    lastValue = emu::getEncoderValue();
  }

  int Encoder_getValue(void)
  {
    return emu::getEncoderValue();
  }

  int Encoder_getChange(void)
  {
    int value = emu::getEncoderValue();
    int change = value - lastValue;
    lastValue = value;
    return change;
  }
}