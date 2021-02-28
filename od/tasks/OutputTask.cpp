#include <od/tasks/OutputTask.h>
#include <od/config.h>
#include <string.h>

namespace od
{

	static inline void zeroOutput(float *dst, uint32_t channel)
	{
		float *end = dst + NUM_OUTPUT_CHANNELS * globalConfig.frameLength;
		dst += channel;
		for (; dst < end; dst += NUM_OUTPUT_CHANNELS)
		{
			*dst = 0;
		}
	}

	static inline void copyToOutputs(float *__restrict__ dst,
																	 const float *__restrict__ src, uint32_t channel)
	{
		const float *end = src + FRAMELENGTH;
		dst += channel;
		for (; src < end; dst += NUM_OUTPUT_CHANNELS, src++)
		{
			*dst = *src;
		}
	}

	OutputTask::OutputTask() : Task("OutputTask")
	{
		own(mOut1);
		own(mOut2);
		own(mOut3);
		own(mOut4);
		own(mMonitor1);
		own(mMonitor2);
		own(mMonitor3);
		own(mMonitor4);
	}

	OutputTask::~OutputTask()
	{
	}

	void OutputTask::process(float *inputs, float *outputs)
	{
		if (mOut1.isConnected())
		{
			float *src = mOut1.buffer();
			copyToOutputs(outputs, src, 0);
			if (mMonitor1.isConnected())
			{
				memcpy(mMonitor1.buffer(), src, FRAMELENGTH * sizeof(float));
			}
		}
		else
		{
			zeroOutput(outputs, 0);
			memset(mMonitor1.buffer(), 0, FRAMELENGTH * sizeof(float));
		}

		if (mOut2.isConnected())
		{
			float *src = mOut2.buffer();
			copyToOutputs(outputs, src, 1);
			if (mMonitor2.isConnected())
			{
				memcpy(mMonitor2.buffer(), src, FRAMELENGTH * sizeof(float));
			}
		}
		else
		{
			zeroOutput(outputs, 1);
			memset(mMonitor2.buffer(), 0, FRAMELENGTH * sizeof(float));
		}

		if (mOut3.isConnected())
		{
			float *src = mOut3.buffer();
			copyToOutputs(outputs, src, 2);
			if (mMonitor3.isConnected())
			{
				memcpy(mMonitor3.buffer(), src, FRAMELENGTH * sizeof(float));
			}
		}
		else
		{
			zeroOutput(outputs, 2);
			memset(mMonitor3.buffer(), 0, FRAMELENGTH * sizeof(float));
		}

		if (mOut4.isConnected())
		{
			float *src = mOut4.buffer();
			copyToOutputs(outputs, src, 3);
			if (mMonitor4.isConnected())
			{
				memcpy(mMonitor4.buffer(), src, FRAMELENGTH * sizeof(float));
			}
		}
		else
		{
			zeroOutput(outputs, 4);
			memset(mMonitor4.buffer(), 0, FRAMELENGTH * sizeof(float));
		}
	}

} /* namespace od */
