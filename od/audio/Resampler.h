#pragma once

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
		float mPhaseDelta = 0.0f;
		float mPhase = 0.0f;

		void updatePhaseDelta();
	};

} /* namespace od */
