/*
 * RunningRMS.h
 *
 *  Created on: 7 Oct 2016
 *      Author: clarkson
 */

#ifndef APP_UTILS_RUNNINGRMS_H_
#define APP_UTILS_RUNNINGRMS_H_

namespace od
{

	class RunningRMS
	{
	public:
		RunningRMS();
		virtual ~RunningRMS();
		bool push(float *buffer, int n);
		void setPeriod(float secs);
		void reset();

		float mRMS = 0;

	private:
		int mCount = 0;
		int mPeriod = 0;
		float mSum = 0;
		int mSumCount = 0;
	};

} /* namespace od */

#endif /* APP_UTILS_RUNNINGRMS_H_ */
