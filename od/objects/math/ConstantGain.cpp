#include <od/objects/math/ConstantGain.h>
#include <od/extras/LookupTables.h>
#include <od/config.h>
#include <math.h>

namespace od
{

	ConstantGain::ConstantGain()
	{
		addInput(mInput);
		addOutput(mOutput);
		addParameter(mGain);
	}

	ConstantGain::~ConstantGain()
	{
	}

	void ConstantGain::setClamp(float threshold)
	{
		mClamp = threshold;
	}

	void ConstantGain::setClampInDecibels(float threshold)
	{
		mClamp = powf(10.0f, threshold * (1.0f / 20.0f));
	}

	void ConstantGain::process()
	{
		float *in = mInput.buffer();
		float *out = mOutput.buffer();
		float gain0 = mPreviousGain;
		float gain1 = mGain.value();

		if (fabs(gain1) < mClamp)
		{
			for (int i = 0; i < FRAMELENGTH; i++)
			{
				out[i] = 0.0f;
			}
		}
		else if (fabs(gain1 - gain0) < 1e-10f)
		{
			// 600 ticks
			for (int i = 0; i < FRAMELENGTH; i++)
			{
				out[i] = in[i] * gain1;
			}
		}
		else
		{
			// 1300 ticks
			float *ramp = LookupTables::FrameOfLinearRamp.mValues.data();

			for (int i = 0; i < FRAMELENGTH; i++)
			{
				float w1 = ramp[i];
				float w0 = 1.0f - w1;
				out[i] = in[i] * (w0 * gain0 + w1 * gain1);
			}
		}

		mPreviousGain = gain1;
	}

} /* namespace od */
