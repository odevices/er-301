/*
 * FastEWMA.h
 *
 *  Created on: 4 Sep 2016
 *      Author: clarkson
 */

#ifndef APP_UTILS_FASTEWMA_H_
#define APP_UTILS_FASTEWMA_H_

namespace od
{

	class FastEWMA
	{
	public:
		FastEWMA();
		virtual ~FastEWMA();

		// The time it takes to reach 63% of a step input.
		inline void setTimeConstant(float sampleRate, float secs)
		{
			// approximation of 1-exp(-1.0/(sampleRate * secs))
			// 4x for neon
			mAlpha = 4.0f / (sampleRate * secs);
		}

		inline void setInitialState(float y)
		{
			mY = y;
		}

		//
		float push(float *buffer, int n);
		float pushMagnitudes(float *buffer, int n);
		float pushDecibels(float *buffer, int n);

	protected:
		float mAlpha = 1.0f;
		float mY = 0.0f;
	};

} /* namespace od */

#endif /* APP_UTILS_FASTEWMA_H_ */
