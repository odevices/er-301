#include <od/objects/math/Multiply.h>
#include <od/config.h>
#include <hal/simd.h>

namespace od
{

	Multiply::Multiply()
	{
		addInput(mLeftInput);
		addInput(mRightInput);
		addOutput(mOutput);
	}

	Multiply::~Multiply()
	{
	}

	void Multiply::process()
	{
		float *left = mLeftInput.buffer();
		float *right = mRightInput.buffer();
		float *out = mOutput.buffer();

		for (int i = 0; i < FRAMELENGTH; i += 4)
		{
			vst1q_f32(out + i, vld1q_f32(left + i) * vld1q_f32(right + i));
		}
	}

} /* namespace od */
