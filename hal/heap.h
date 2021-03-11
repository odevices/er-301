#pragma once

#include <stdint.h>
#include <stddef.h>

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
  void * Heap_memalign(size_t align, size_t size);
  void * Heap_malloc(size_t size);
  void * Heap_calloc(size_t nmemb, size_t size);
  void * Heap_realloc(void * ptr, size_t size);
  void Heap_free(void* ptr);

#ifdef __cplusplus
}
#endif
