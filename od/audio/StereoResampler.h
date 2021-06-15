#pragma once

#include <od/audio/Resampler.h>

namespace od
{

	class StereoResampler : public Resampler
	{
	public:
		StereoResampler();
		virtual ~StereoResampler();

		int nextFrame(float *in, float *out);
		int nextFrame(float *in, float *left, float *right);

	private:
		float mFifo[6] = {
				0.0f,
				0.0f,
				0.0f,
				0.0f,
				0.0f,
				0.0f,
		};
	};

} /* namespace od */
