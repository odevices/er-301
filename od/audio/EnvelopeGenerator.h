/*
 * EnvelopeGenerator.h
 *
 *  Created on: 9 Jun 2016
 *      Author: clarkson
 */

#ifndef APP_AUDIO_ENVELOPEGENERATOR_H_
#define APP_AUDIO_ENVELOPEGENERATOR_H_

#include <od/extras/ReferenceCounted.h>

namespace od
{

	class EnvelopeGenerator : public ReferenceCounted
	{
	public:
		EnvelopeGenerator();
		virtual ~EnvelopeGenerator();

		virtual void on()
		{
			mCurrentValue = 1.0f;
		}

		virtual void off()
		{
			mCurrentValue = 0.0f;
		}

		virtual float next()
		{
			return mCurrentValue;
		}

		inline float getCurrentValue()
		{
			return mCurrentValue;
		}

		void setTriggerThreshold(float threshold)
		{
			mThreshold = threshold;
		}

		void setSampleRate(float sr)
		{
			mSamplePeriod = 1.0f / sr;
		}

		float mThreshold = 0.1f;
		float mSamplePeriod = 0.001f;
		float mCurrentValue = 0.0f;
	};

} /* namespace od */

#endif /* APP_AUDIO_ENVELOPEGENERATOR_H_ */
