#include <core/objects/filters/FixedFilter.h>
#include <od/config.h>

namespace od
{

	FixedFilter::FixedFilter(int order)
	{
		addInput(mInput);
		addOutput(mOutput);
		order /= 2;
		if (order < 1)
			mOrder = 1;
		else
			mOrder = order;
		mpBiquadFilters = new BiquadFilter[mOrder];
		setLowPass(0.1f);
	}

	FixedFilter::~FixedFilter()
	{
		delete[] mpBiquadFilters;
	}

	void FixedFilter::process()
	{
		float *in = mInput.buffer();
		float *out = mOutput.buffer();
		float *end = out + globalConfig.frameLength;
		int i;

		while (out < end)
		{
			*out = mpBiquadFilters[0].push(*in);
			for (i = 1; i < mOrder; i++)
			{
				*out = mpBiquadFilters[i].push(*out);
			}
			out++;
			in++;
		}
	}

} /* namespace od */
