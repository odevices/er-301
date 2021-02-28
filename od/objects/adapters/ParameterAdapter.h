#pragma once

#include <od/objects/Object.h>
#include <limits>

namespace od
{

	// Imitates a combined GainBias and MinMax object while sampling Inlet to a Parameter.
	class ParameterAdapter : public Object
	{
	public:
		ParameterAdapter();
		virtual ~ParameterAdapter();

#ifndef SWIGLUA
		virtual void process();
		Inlet mInput{"In"};
		Outlet mProbeOutput{"Out"};
		Parameter mOutput{"Out"};
		Parameter mGain{"Gain"};
		Parameter mBias{"Bias"};
		Parameter mMinimum{"Min"};
		Parameter mMaximum{"Max"};
		Parameter mCenter{"Center"};
#endif

		void clamp(float lower, float upper)
		{
			mLowerClamp = lower;
			mUpperClamp = upper;
		}

	private:
		float mLowerClamp = std::numeric_limits<float>::lowest();
		float mUpperClamp = std::numeric_limits<float>::max();

		// range measurements
		int mCount = 0;
		int mPeriod = 0;
		float mNextMin = 0;
		float mNextMax = 0;

		void calculateProbeOutput();
		float mPreviousGain = 0.0f;
		float mPreviousBias = 0.0f;
	};

} /* namespace od */
