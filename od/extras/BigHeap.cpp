#include <od/extras/BigHeap.h>
#include <hal/log.h>
#include <hal/heap.h>

namespace od
{

  BigHeap::BigHeap()
  {
    uintptr_t heapStart = Heap_getUnusedMemoryStart();
    uint32_t heapSize = Heap_getUnusedMemorySize();
    mAllocator.allocateFrom((char *)heapStart, heapSize);
  }

  BigHeap &BigHeap::singleton()
  {
    static BigHeap bigHeap;
    return bigHeap;
  }

  char *BigHeap::allocate(int bytes)
  {
    return singleton().mAllocator.allocate(bytes);
  }

  char *BigHeap::allocateZeroed(int bytes)
  {
    return singleton().mAllocator.allocateZeroed(bytes);
  }

  void BigHeap::free(char *ptr)
  {
    return singleton().mAllocator.free(ptr);
  }

  int BigHeap::size(int unit)
  {
    return singleton().mAllocator.size() / unit;
  }

  int BigHeap::remaining(int unit)
  {
    return singleton().mAllocator.remaining() / unit;
  }

  int BigHeap::largest(int unit)
  {
    return singleton().mAllocator.largest() / unit;
  }

  void BigHeap::print()
  {
    logInfo("BigHeap: size=%dMB free=%dMB", size(1024 * 1024), remaining(1024 * 1024));
    singleton().mAllocator.printSections();
  }

} // namespace od