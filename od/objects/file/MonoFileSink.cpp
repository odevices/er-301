#include <od/objects/file/MonoFileSink.h>
#include <od/config.h>
#include <string.h>

namespace od
{

	MonoFileSink::MonoFileSink() : FileSink(1)
	{
		addInput(mInput);
	}

	MonoFileSink::~MonoFileSink()
	{
	}

	void MonoFileSink::process()
	{
		if (mStarted && mWriter.mIsOpen)
		{
			float *in = mInput.buffer();
			float *buffer = getWriteBuffer();
			if (buffer)
			{
				memcpy(buffer, in, FRAMELENGTH * sizeof(float));
				mTotalSeconds += globalConfig.framePeriod;
				putWriteBuffer();
			}
		}
	}

} /* namespace od */
