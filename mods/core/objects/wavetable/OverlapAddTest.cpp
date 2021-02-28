#include <core/objects/wavetable/OverlapAddTest.h>
#include <od/config.h>

namespace od
{

	OverlapAddTest::OverlapAddTest()
	{
		addInput(mInput);
		addOutput(mOutput);
		addParameter(mHop);
		int fftsize = globalConfig.frameLength;
		int nbins = fftsize / 2 + 1;
		mFFT.allocate(fftsize);
		mSpectrum.resize(2 * nbins);
	}

	OverlapAddTest::~OverlapAddTest()
	{
	}

	void OverlapAddTest::process()
	{
		float *in = mInput.buffer();
		float *out = mOutput.buffer();
		int hop = mHop.roundValue();

		mFFT.compute((complex_float_t *)mSpectrum.data(), in);
		for (int i = 2 * hop; i < FRAMELENGTH; i += 2 * hop)
		{
			mSpectrum[i] = 0;
			mSpectrum[i + 1] = 0;
		}
		mFFT.computeInverse(out, (complex_float_t *)mSpectrum.data());
	}

} /* namespace od */
