#pragma once

#ifdef __cplusplus
extern "C"
{
#endif

  void Encoder_init(void);

  // requires 130ns
  int Encoder_getValue(void);
  int Encoder_getChange(void);

#ifdef __cplusplus
}
#endif
