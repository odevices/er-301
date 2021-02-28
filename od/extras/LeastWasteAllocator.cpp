#include <od/extras/LeastWasteAllocator.h>
#include <hal/log.h>
#include <cstring>
#include <algorithm>

namespace od
{

  LeastWasteAllocator::LeastWasteAllocator()
  {
  }

  LeastWasteAllocator::LeastWasteAllocator(char *ptr, uint32_t sizeInBytes)
  {
    allocateFrom(ptr, sizeInBytes);
  }

  void LeastWasteAllocator::allocateFrom(char *ptr, uint32_t sizeInBytes)
  {
    if (ptr && sizeInBytes > 0)
    {
      mpHeap = ptr;
      logAssert(mpHeap);
      mFree.emplace_back(mpHeap, sizeInBytes, 0);
      mHeapSize = sizeInBytes;
      mRemaining = sizeInBytes;
    }
    else
    {
      mpHeap = 0;
      mHeapSize = 0;
      mRemaining = 0;
    }

    mNeedToMerge = false;
  }

  LeastWasteAllocator::~LeastWasteAllocator()
  {
  }

  char *LeastWasteAllocator::allocateZeroed(int requestedSizeInBytes)
  {
    char *ptr = allocate(requestedSizeInBytes);
    if (ptr)
    {
      memset(ptr, 0, requestedSizeInBytes);
    }
    return ptr;
  }

  char *LeastWasteAllocator::allocate(int requestedSizeInBytes)
  {
    logDebug(1, "LeastWasteAllocator(%d free, %d used): about to allocate %d KB",
             remaining() / 1024, used() / 1024, requestedSizeInBytes / 1024);

    if (requestedSizeInBytes == 0 || requestedSizeInBytes > mRemaining)
      return 0;

    if (mNeedToMerge)
    {
      mergeFreeSections();
    }

    // search through the freed sections
    // choose the section large enough to fit with the least amount of waste
    std::vector<Section>::iterator i, best = mFree.end();
    int leastWaste = mRemaining;
    for (i = mFree.begin(); i != mFree.end(); i++)
    {
      Section &section = *i;
      int waste = section.mSizeInBytes - requestedSizeInBytes;
      if (waste >= 0 && waste < leastWaste)
      {
        leastWaste = waste;
        best = i;
      }
    }

    if (best == mFree.end())
    {
      // Failed to find a section large enough to satisfy the request.
      return 0;
    }

    // Create a section of the desired size at the beginning of the free section.
    Section &found = *best;
    mAllocated.emplace_back(found.mpData, requestedSizeInBytes, 1);
    if (found.mSizeInBytes == requestedSizeInBytes)
    {
      // The free section is completely used, so remove it.
      mFree.erase(best);
    }
    else
    {
      // Shrink and shift the free section.
      found.mSizeInBytes -= requestedSizeInBytes;
      found.mpData += requestedSizeInBytes;
    }
    mRemaining -= requestedSizeInBytes;

    Section *section = &mAllocated.back();
    logDebug(1, "LeastWasteAllocator(%d free, %d used): allocated %d KB",
             remaining() / 1024, used() / 1024, requestedSizeInBytes / 1024);
    logDebug(1, "Section: %s{offset=%d, size=%dKB}",
             section->mStatus == 0 ? "Free" : "Used",
             section->mpData - mpHeap, section->mSizeInBytes / 1024);
    return section->mpData;
  }

  void LeastWasteAllocator::free(char *ptr)
  {
    // Look for the given section in the list of allocated sections.
    auto i = std::find_if(mAllocated.begin(), mAllocated.end(),
                          [ptr](const Section &x) { return x.mpData == ptr; });
    if (i != mAllocated.end())
    {
      Section &section = *i;
      logDebug(1, "LeastWasteAllocator(%d free, %d used): about to free %d KB",
               remaining() / 1024, used() / 1024, section.mSizeInBytes / 1024);
      logDebug(1, "Section: %s{offset=%d, size=%dKB}",
               section.mStatus == 0 ? "Free" : "Used",
               section.mpData - mpHeap, section.mSizeInBytes / 1024);

      // Found it, so mark it as free and move it to the list of free sections.
      section.mStatus = 0;
      mRemaining += section.mSizeInBytes;
      mFree.emplace_back(section);
      mAllocated.erase(i);
      // Don't merge free sections yet because there might be more deallocations coming.
      mNeedToMerge = true;
    }
    else
    {
      logError("Not a pointer to an allocated memory section.");
    }
  }

  void LeastWasteAllocator::mergeFreeSections()
  {
    logDebug(1, "LeastWasteAllocator(%dMB free, %dMB used): merge free sections",
             remaining() / 1024 / 1024, used() / 1024 / 1024);

    if (mFree.size() < 2)
      return;

    // sort the free sections
    std::sort(mFree.begin(), mFree.end());

    // step through, merging pairwise
    std::vector<Section>::iterator i, j;
    i = mFree.begin();
    j = i + 1;
    while (j != mFree.end())
    {
      Section &A = *i;
      Section &B = *j;
      if (A.mpData + A.mSizeInBytes == B.mpData)
      {
        // merge A and B
        A.mSizeInBytes += B.mSizeInBytes;
        mFree.erase(j);
        j = i + 1;
      }
      else
      {
        i++;
        j++;
      }
    }

    mNeedToMerge = false;
  }

  int LeastWasteAllocator::largest()
  {
    if (mNeedToMerge)
    {
      mergeFreeSections();
    }

    int sizeInBytes = 0;
    for (Section &section : mFree)
    {
      if (section.mSizeInBytes > sizeInBytes)
      {
        sizeInBytes = section.mSizeInBytes;
      }
    }

    return sizeInBytes;
  }

  void LeastWasteAllocator::printSections()
  {

    if (mNeedToMerge)
    {
      mergeFreeSections();
    }

    // combine and sort all sections
    std::vector<Section> all;
    all.reserve(mFree.size() + mAllocated.size());
    all.insert(all.end(), mAllocated.begin(), mAllocated.end());
    all.insert(all.end(), mFree.begin(), mFree.end());
    std::sort(all.begin(), all.end());

    int i = 0;
    for (Section &section : all)
    {
      logInfo("Section(%d): %s{offset=%d, size=%dKB}", i++,
              section.mStatus == 0 ? "Free" : "Used",
              section.mpData - mpHeap,
              section.mSizeInBytes / 1024);
    }
  }

} /* namespace od */
