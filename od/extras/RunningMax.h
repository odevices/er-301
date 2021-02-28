/*
 * RunningMax.h
 *
 *  Created on: 7 Oct 2016
 *      Author: clarkson
 */

#ifndef APP_UTILS_RUNNINGMAX_H_
#define APP_UTILS_RUNNINGMAX_H_

namespace od
{

	class RunningMax
	{
	public:
		RunningMax();
		virtual ~RunningMax();

		bool push(float *buffer, int n);
		void setPeriod(float secs);
		void reset();

		float mMaximum = 0;

	private:
		int mCount = 0;
		int mPeriod = 0;
		float mNextMax = 0;
	};

} /* namespace od */

#endif /* APP_UTILS_RUNNINGMAX_H_ */
