/*
 * ZeroCrossingDetector.cpp
 *
 *  Created on: 2017/06/06
 *      Author: clarkson
 */

#include <core/objects/timing/ZeroCrossingDetector.h>
#include <od/config.h>

namespace od
{

	ZeroCrossingDetector::ZeroCrossingDetector()
	{
		addInput(mGateInput);
		addInput(mAudioInput);
		addOutput(mGateOutput);
	}

	ZeroCrossingDetector::~ZeroCrossingDetector()
	{
	}

#define WAITING 0
#define PRIMED 1
#define TRIGGERED 2

	void ZeroCrossingDetector::process()
	{
		float *in = mGateInput.buffer();
		float *out = mGateOutput.buffer();
		float *audio = mAudioInput.buffer();

		for (int i = 0; i < FRAMELENGTH; i++)
		{
			switch (mState)
			{
			case WAITING:
				out[i] = 0.0f;
				if (in[i] > 0.1f)
				{
					mState = PRIMED;
				}
				break;
			case PRIMED:
				out[i] = 0.0f;
				if ((mLastValue < 0 && audio[i] > 0.0f) || (mLastValue > 0.0f && audio[i] < 0.0f))
				{
					mState = TRIGGERED;
				}
				break;
			case TRIGGERED:
				out[i] = 1.0f;
				if (in[i] < 0.1f)
				{
					mState = WAITING;
				}
				break;
			}

			mLastValue = audio[i];
		}
	}

} /* namespace od */
