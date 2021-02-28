#pragma once

#include <od/extras/LeastWasteAllocator.h>

namespace od
{

  class BigHeap
  {
  public:
#ifndef SWIGLUA
    static char *allocate(int bytes);
    static char *allocateZeroed(int bytes);
    static void free(char *ptr);
#endif
    static int size(int unit);
    static int remaining(int unit);
    static int largest(int unit);
    static void print();

  private:
    BigHeap();
    static BigHeap &singleton();

    LeastWasteAllocator mAllocator;
  };

} // namespace od