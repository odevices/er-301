#include <od/objects/measurement/Stress.h>
#include <hal/rng.h>
#include <od/config.h>
#include <hal/ops.h>

namespace od
{

	Stress::Stress()
	{
		addInput(mInput);
		addOutput(mOutput);
		addParameter(mLoad);
	}

	Stress::~Stress()
	{
	}

#if 0
#define STRESS_FACTOR (1e7f / 240.0f * (128.0f / FRAMELENGTH))

void Stress::process() {
	float * in = mInput.buffer();
	float * out = mOutput.buffer();
	float p = 100 * MIN(1, MAX(0, mLoad.value()));
	int n = (int) (mAdjustment * p * globalConfig.framePeriod * STRESS_FACTOR);

	for (int j = 0; j < n; j++) {
		// 240 ticks
		for (int i = 0; i < FRAMELENGTH; i++) {
			out[i] = in[i];
		}
	}

	for (int i = 0; i < FRAMELENGTH; i++) {
		out[i] = in[i];
	}

}

#else

#define STRESS_FACTOR (1e7f / 21000)
	static uint64_t stress_sum = 0.0f;

	void Stress::process()
	{
		float *in = mInput.buffer();
		float *out = mOutput.buffer();
		float p = 100 * MIN(1, MAX(0, mLoad.value()));
		int n = (int)(mAdjustment * p * globalConfig.framePeriod * STRESS_FACTOR);

		stress_sum = 0;
		for (int j = 0; j < n; j++)
		{
			// 21000 ticks
			stress_sum += Rng_read32();
		}

		for (int i = 0; i < FRAMELENGTH; i++)
		{
			out[i] = in[i];
		}
	}
#endif

} /* namespace od */
