/*
 * MonoConvolution.cpp
 *
 *  Created on: 13 Sep 2017
 *      Author: clarkson
 */

#include <core/objects/filters/MonoConvolution.h>
#include <od/config.h>
#include <hal/ops.h>

namespace od
{

	MonoConvolution::MonoConvolution()
	{
		addInput(mInput);
		addOutput(mOutput);
	}

	MonoConvolution::~MonoConvolution()
	{
		if (mpSample)
		{
			convolve.clearIR();
			mpSample->release();
		}
	}

	void MonoConvolution::setSample(Sample *sample)
	{
		if (mpSample)
		{
			convolve.clearIR();
			mpSample->release();
		}
		mpSample = sample;
		if (mpSample)
		{
			mpSample->attach();
			int max = 72000 - globalConfig.sampleRate / 2;
			convolve.setIR(mpSample->mpData, MIN(max, (int)mpSample->mSampleCount),
										 mpSample->mChannelCount);
		}
	}

	void MonoConvolution::process()
	{
		float *in = mInput.buffer();
		float *out = mOutput.buffer();
		convolve.process(in, out);
	}

} /* namespace od */
