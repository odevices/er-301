/*
 * SineRamp.h
 *
 *  Created on: 27 Feb 2017
 *      Author: clarkson
 */

#ifndef APP_UTILS_COSINERAMP_H_
#define APP_UTILS_COSINERAMP_H_

#include <od/extras/LookupTables.h>

namespace od
{

	class CosineRamp
	{
	public:
		CosineRamp();
		virtual ~CosineRamp();

		void setLength(int n)
		{
			if (mLength != n)
			{
				mLength = n;
				mStep = 0.5f / (n > 0 ? n : 1);
			}
		}

		void reset(float start, float stop)
		{
			mPhase = 0.25f;
			mGain = 0.5f * (start - stop);
			mGoal = stop;
			mValue = start;
		}

		void set(float value)
		{
			mPhase = 1.0f;
			mGoal = value;
			mValue = value;
		}

		inline float step()
		{
			if (mPhase < 0.75f)
			{
				mValue = mGain * (1.0f + LookupTables::Sine.getNoCheckInterpolated(mPhase)) + mGoal;
				mPhase += mStep;
				return mValue;
			}
			else
			{
				return mGoal;
			}
		}

		int mLength = 1;
		float mPhase = 0.75f;
		float mStep = 0.5f;
		float mGain = 0.0f;
		float mValue = 0.0f;
		float mGoal = 0.0f;
	};

} /* namespace od */

#endif /* APP_UTILS_COSINERAMP_H_ */
