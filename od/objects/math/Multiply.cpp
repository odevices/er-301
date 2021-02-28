#include <od/objects/math/Multiply.h>
#include <od/config.h>

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

#if 0
	while (out < end) {
		float32x4_t p1 = vld1q_f32(left);
		float32x4_t p2 = vld1q_f32(right);
		vst1q_f32(out, vmulq_f32(p1, p2));
		out += 4;
		left += 4;
		right += 4;
	}
#else
		for (int i = 0; i < FRAMELENGTH; i++)
		{
			out[i] = left[i] * right[i];
		}
#endif
	}

} /* namespace od */
