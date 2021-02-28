#include <od/objects/file/StereoFileSink.h>
#include <od/config.h>

namespace od
{

	StereoFileSink::StereoFileSink() : FileSink(2)
	{
		addInput(mLeftInput);
		addInput(mRightInput);
	}

	StereoFileSink::~StereoFileSink()
	{
	}

	void StereoFileSink::process()
	{
		if (mStarted && mWriter.mIsOpen)
		{
			float *buffer = getWriteBuffer();
			if (buffer)
			{
				float *left = mLeftInput.buffer();
				float *right = mRightInput.buffer();
				float *end = left + FRAMELENGTH;
				while (left < end)
				{
					*buffer = *left;
					buffer++;
					*buffer = *right;
					buffer++;
					left++;
					right++;
				}
				mTotalSeconds += globalConfig.framePeriod;
				putWriteBuffer();
			}
		}
	}
} /* namespace od */
