/*
 * Loudness.h
 *
 *  Created on: 4 Sep 2016
 *      Author: clarkson
 */

#ifndef APP_UTILS_LOUDNESS_H_
#define APP_UTILS_LOUDNESS_H_

namespace od
{

	class Loudness
	{
	public:
		Loudness();
		virtual ~Loudness();

		float push(float *buffer, int n);
		void reset();

	protected:
		float mMaximum = 0;
		float mDecibels = -160;
		int mCount = 0;
		int mPeriod = 0;
		float mNextMax = 0;
	};

} /* namespace od */

#endif /* APP_UTILS_LOUDNESS_H_ */
