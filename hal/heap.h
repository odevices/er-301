#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

  void Heap_init(void);
  void Heap_print(void);
  int Heap_getSize(int units);
  int Heap_getFreeSize(int units);
  uintptr_t Heap_getUnusedMemoryStart();
  uint32_t Heap_getUnusedMemorySize();

#ifdef __cplusplus
}
#endif
