#include <od/extras/SystemBuffer.h>
#include <hal/constants.h>
#include <stdlib.h>

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
                posix_memalign((void **)&mpData, CACHELINE_SIZE_MAX, n);
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
			free(mpData);
			mpData = 0;
			mSizeInBytes = 0;
		}
	}

} /* namespace od */
