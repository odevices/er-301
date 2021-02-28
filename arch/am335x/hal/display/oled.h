#pragma once

#include <hal/constants.h>

#ifdef __cplusplus
extern "C"
{
#endif

  void mainInit(void);
  void mainDisplayOn(void);
  void mainTestAnimation(void);
  void mainTestAnimationDoubleBuffer(void);
  void mainDrawBox(void);
  void mainFill(uint32_t value);
  void mainFlip(void);

  void subInit(void);
  void subDisplayOn(void);
  void subFill(uint32_t fill);
  void subDrawBox(void);

#ifdef __cplusplus
}
#endif
