#include <od/audio/Resampler.h>
#include <od/config.h>
#include <hal/ops.h>

namespace od
{

	Resampler::Resampler()
	{
		updatePhaseDelta();
	}

	Resampler::~Resampler()
	{
	}

	void Resampler::updatePhaseDelta()
	{
		mPhaseDelta = mSpeed * mSpeedAdjustment;
	}

	void Resampler::setInputRate(float rate)
	{
		mSpeedAdjustment = MAX(0, rate) * globalConfig.samplePeriod;
		updatePhaseDelta();
	}

	void Resampler::setSpeed(float speed)
	{
		mSpeed = MAX(0, speed);
		updatePhaseDelta();
	}

	int Resampler::required()
	{
		return (int)(globalConfig.frameLength * mPhaseDelta + 1);
	}

	int Resampler::required(float speed)
	{
		float dPhase = speed * mSpeedAdjustment;
		return (int)(globalConfig.frameLength * dPhase + 1);
	}

} /* namespace od */
