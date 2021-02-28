/*
 * Resampler.h
 *
 *  Created on: 10 May 2017
 *      Author: clarkson
 */

#ifndef APP_AUDIO_RESAMPLER_H_
#define APP_AUDIO_RESAMPLER_H_

namespace od
{

	/*
 * Frame-based
 * Constant speed over frame
 * Positive speed only
 */

	class Resampler
	{
	public:
		Resampler();
		virtual ~Resampler();

		void setInputRate(float rate);
		void setSpeed(float speed);
		int required();
		int required(float speed);

	protected:
		float mSpeed = 1.0f;
		float mSpeedAdjustment = 1.0f;
		float mPhaseDelta;
		float mPhase = 0.0f;

		void updatePhaseDelta();
	};

} /* namespace od */

#endif /* APP_AUDIO_RESAMPLER_H_ */
