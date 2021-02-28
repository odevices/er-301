/*
 * StereoConvolution.cpp
 *
 *  Created on: 14 Sep 2017
 *      Author: clarkson
 */

#include <core/objects/filters/StereoConvolution.h>
#include <od/config.h>
#include <hal/ops.h>

namespace od
{

	StereoConvolution::StereoConvolution()
	{
		addInput(mLeftInput);
		addInput(mRightInput);
		addOutput(mLeftOutput);
		addOutput(mRightOutput);
	}

	StereoConvolution::~StereoConvolution()
	{
		if (mpSample)
		{
			mLeftFilter.clearIR();
			mRightFilter.clearIR();
			mpSample->release();
		}
	}

	void StereoConvolution::setSample(Sample *sample)
	{
		if (mpSample)
		{
			mLeftFilter.clearIR();
			mRightFilter.clearIR();
			mpSample->release();
		}
		mpSample = sample;
		if (mpSample)
		{
			mpSample->attach();
			int max = 36000 - globalConfig.sampleRate / 4;
			mRightFilter.setIR(mpSample->mpData, MIN(max, (int)mpSample->mSampleCount),
												 mpSample->mChannelCount);
			mLeftFilter.shareIR(mRightFilter);
		}
	}

	void StereoConvolution::process()
	{
		float *inL = mLeftInput.buffer();
		float *outL = mLeftOutput.buffer();
		float *inR = mRightInput.buffer();
		float *outR = mRightOutput.buffer();
		mLeftFilter.process(inL, outL);
		mRightFilter.process(inR, outR);
	}

} /* namespace od */
