#include <od/objects/math/Gain.h>
#include <od/config.h>
#include <hal/simd.h>

namespace od
{

  Gain::Gain()
  {
    addInput(mInput);
    addInput(mGain);
    addOutput(mOutput);
    addParameter(mBaseGain);
    mBaseGain.hardSet(1.0f);
  }

  Gain::~Gain()
  {
  }

  void Gain::process()
  {
    float *in = mInput.buffer();
    float *gain = mGain.buffer();
    float *out = mOutput.buffer();
    float baseGain = mBaseGain.value();

#if 0
	float32x4_t Kgain = vdupq_n_f32(baseGain);
	float * end = out + globalConfig.frameLength;
	while (out < end) {
		float32x4_t Vin = vld1q_f32(in);
		float32x4_t Vgain = vaddq_f32(vld1q_f32(gain), Kgain);
		vst1q_f32(out, vmulq_f32(Vin, Vgain));
		out += 4;
		in += 4;
		gain += 4;
	}

#else
    for (int i = 0; i < FRAMELENGTH; i++)
    {
      out[i] = in[i] * (gain[i] + baseGain);
    }
#endif
  }

} /* namespace od */
