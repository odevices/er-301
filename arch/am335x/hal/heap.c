#include <hal/log.h>
#include <xdc/runtime/Memory.h>
#include <malloc.h>

// These symbols are defined in the linker script.
extern uintptr_t __unused_memory_start__;
extern uintptr_t __unused_memory_end__;

uintptr_t Heap_getUnusedMemoryStart()
{
  return (uintptr_t)&__unused_memory_start__;
}

uint32_t Heap_getUnusedMemorySize()
{
  uintptr_t heapStart = (uintptr_t)&__unused_memory_start__;
  uintptr_t heapEnd = (uintptr_t)&__unused_memory_end__;
  uint32_t heapSize = heapEnd - heapStart;
  return heapSize;
}

void Heap_print(void)
{
  Memory_Stats stats;
  Memory_getStats(Memory_defaultHeapInstance, &stats);
  logInfo("Heap_print: using %dMB of %dMB largest=%dMB",
          (stats.totalSize - stats.totalFreeSize) / (1024 * 1024),
          stats.totalSize / (1024 * 1024),
          stats.largestFreeSize / (1024 * 1024));
}

int Heap_getSize(int units)
{
  Memory_Stats stats;
  Memory_getStats(Memory_defaultHeapInstance, &stats);
  return (int)(stats.totalSize / units);
}

int Heap_getFreeSize(int units)
{
  Memory_Stats stats;
  Memory_getStats(Memory_defaultHeapInstance, &stats);
  return (int)(stats.totalFreeSize / units);
}

void Heap_init()
{
}

void *Heap_memalign(size_t align, size_t size)
{
  return memalign(align, size);
}

void *Heap_malloc(size_t size)
{
  return malloc(size);
}

void *Heap_calloc(size_t nmemb, size_t size)
{
  return calloc(nmemb, size);
}

void *Heap_realloc(void *ptr, size_t size)
{
  return realloc(ptr, size);
}

void Heap_free(void *ptr)
{
  free(ptr);
}