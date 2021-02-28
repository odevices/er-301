/*
 * PopReducer.cpp
 *
 *  Created on: 14 Sep 2016
 *      Author: clarkson
 */

#include <core/objects/env/PopReducer.h>
#include <od/config.h>

namespace od
{

	PopReducer::PopReducer()
	{
		addInput(mInput);
		addInput(mTrigger);
		addOutput(mOutput);
		addParameter(mTime);
		mTime.hardSet(1.0f);
	}

	PopReducer::~PopReducer()
	{
	}

	// 600 ticks for 128 samples
	void PopReducer::process()
	{
		float *in = mInput.buffer();
		float *trigger = mTrigger.buffer();
		float *out = mOutput.buffer();
		float *end = out + globalConfig.frameLength;

		float ms = mTime.value();
		float dPhase = 1000.0f / (ms * globalConfig.sampleRate);

		while (out < end)
		{
			if (*trigger > 0.0f)
			{
				mPhase = 0.5f;
			}

			if (mPhase < 1.0f)
			{
				*out = mPhase * (*in);
				mPhase += dPhase;
			}
			else
			{
				*out = *in;
			}

			out++;
			in++;
			trigger++;
		}
	}

} /* namespace od */
