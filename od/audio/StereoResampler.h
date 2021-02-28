/*
 * StereoResampler.h
 *
 *  Created on: 10 May 2017
 *      Author: clarkson
 */

#ifndef APP_AUDIO_STEREORESAMPLER_H_
#define APP_AUDIO_STEREORESAMPLER_H_

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

#endif /* APP_AUDIO_STEREORESAMPLER_H_ */
