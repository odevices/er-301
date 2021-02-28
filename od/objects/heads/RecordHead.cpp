#include <od/objects/heads/RecordHead.h>
#include <math.h>

namespace od
{

	RecordHead::RecordHead()
	{
	}

	RecordHead::~RecordHead()
	{
	}

	void RecordHead::zeroBuffer()
	{
		if (mpSample)
		{
			mpSample->zero();
		}
	}

} /* namespace od */
