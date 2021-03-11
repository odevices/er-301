#include <od/extras/SystemBuffer.h>
#include <hal/constants.h>
#include <hal/heap.h>

namespace od
{

	SystemBuffer::SystemBuffer()
	{
	}

	SystemBuffer::~SystemBuffer()
	{
		deallocate();
	}

	bool SystemBuffer::allocate(int n)
	{
		deallocate();
		mpData = (char *)Heap_memalign(CACHELINE_SIZE_MAX, n);
		if (mpData)
		{
			mSizeInBytes = n;
			return true;
		}
		else
		{
			return false;
		}
	}

	void SystemBuffer::deallocate()
	{
		if (mpData)
		{
			Heap_free(mpData);
			mpData = 0;
			mSizeInBytes = 0;
		}
	}

} /* namespace od */
