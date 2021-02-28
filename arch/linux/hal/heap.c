#include <hal/heap.h>
#include <hal/log.h>
#include <malloc.h>

typedef struct
{
  uintptr_t unusedMemoryStart;
  uint32_t unusedMemorySize;
} Local;

static Local local;

uintptr_t Heap_getUnusedMemoryStart()
{
  return local.unusedMemoryStart;
}

uint32_t Heap_getUnusedMemorySize()
{
  return local.unusedMemorySize;
}

void Heap_print(void)
{
  logInfo("Heap_print: not implemented.");
}

int Heap_getSize(int units)
{
  return (int)(0 / units);
}

int Heap_getFreeSize(int units)
{
  return (int)(0 / units);
}

void Heap_init()
{
  local.unusedMemorySize = 487 * 1024 * 1024;
  local.unusedMemoryStart = (uintptr_t)malloc(local.unusedMemorySize);
}
