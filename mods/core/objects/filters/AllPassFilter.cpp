#include <core/objects/filters/AllPassFilter.h>
#include <od/config.h>

namespace od
{

	AllPassFilter::AllPassFilter()
	{
		addInput(mInput);
		addInput(mFrequency);
		addInput(mFeedback);
		addOutput(mOutput);
		addParameter(mOrder);

		for (int i = 0; i < ALLPASSFILTER_MAX_ORDER; i++)
		{
			mYState[i] = 0.0f;
			mXState[i] = 0.0f;
		}
	}

	AllPassFilter::~AllPassFilter()
	{
	}

	void AllPassFilter::process()
	{
		//float * in = mInput.buffer();
		float *out = mOutput.buffer();
		//float * feedback = mFeedback.buffer();
		//float * freq = mFrequency.buffer();
		float *end = out + globalConfig.frameLength;

		int order = mOrder.roundTarget();
		if (order < 1)
		{
			order = 1;
		}
		else if (order > ALLPASSFILTER_MAX_ORDER)
		{
			order = ALLPASSFILTER_MAX_ORDER;
		}

		while (out < end)
		{
			out++;
		}
	}

} /* namespace od */
