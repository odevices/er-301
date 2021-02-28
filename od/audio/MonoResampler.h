#pragma once

#include <od/audio/Resampler.h>

namespace od
{

	class MonoResampler : public Resampler
	{
	public:
		MonoResampler();
		virtual ~MonoResampler();

		int nextFrame(float *in, float *out);

	protected:
		float mFifo[3] = {0, 0, 0};
		float mNeonPhase[4] = {0.0f, 0.0f, 0.0f, 0.0f};
		float mNeonRecent0[4] = {0.0f, 0.0f, 0.0f, 0.0f};
		float mNeonRecent1[4] = {0.0f, 0.0f, 0.0f, 0.0f};
		float mNeonRecent2[4] = {0.0f, 0.0f, 0.0f, 0.0f};

		inline float *incrementPhase(float *in);
	};

} /* namespace od */
