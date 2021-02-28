#pragma once

#include <list>
#include <vector>
#include <stdint.h>

namespace od
{

  class LeastWasteAllocator
  {
  public:
    LeastWasteAllocator();
    LeastWasteAllocator(char *ptr, uint32_t sizeInBytes);
    virtual ~LeastWasteAllocator();

    void allocateFrom(char *ptr, uint32_t sizeInBytes);
    char *allocate(int bytes);
    char *allocateZeroed(int bytes);
    void free(char *ptr);

    inline int size()
    {
      return mHeapSize;
    }

    inline int remaining()
    {
      return mRemaining;
    }

    inline int used()
    {
      return size() - remaining();
    }

    int largest();
    void printSections();

  private:
    struct Section
    {
      Section() : mpData(0), mSizeInBytes(0), mStatus(0)
      {
      }
      Section(char *ptr, int size, int status) : mpData(ptr), mSizeInBytes(size), mStatus(status)
      {
      }

      // Define an ordering relation on sections for sorting.
      const bool operator<(const Section &rhs)
      {
        return mpData < rhs.mpData;
      }

      char *mpData;
      int mSizeInBytes;
      int mStatus;
    };

    std::vector<Section> mFree;
    std::list<Section> mAllocated;
    char *mpHeap = 0;
    int mHeapSize = 0;
    int mRemaining = 0;
    bool mNeedToMerge = false;

    void mergeFreeSections();
  };

} /* namespace od */
