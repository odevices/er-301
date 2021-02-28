#pragma once

#ifdef __cplusplus
extern "C"
{
#endif

  void TLS_set(void *ptr);
  void *TLS_get();

  void TLS_setName(const char *ptr);
  const char *TLS_getName();

#ifdef __cplusplus
}
#endif
