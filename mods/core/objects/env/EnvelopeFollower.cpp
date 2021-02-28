#include <core/objects/env/EnvelopeFollower.h>
#include <od/config.h>
#include <hal/simd.h>
#include <hal/ops.h>
#include <math.h>

namespace od
{

  EnvelopeFollower::EnvelopeFollower()
  {
    addInput(mInput);
    addOutput(mOutput);
    addParameter(mAttack);
    addParameter(mRelease);
  }

  EnvelopeFollower::~EnvelopeFollower()
  {
  }

  void EnvelopeFollower::process()
  {
    float *in = mInput.buffer();
    float *out = mOutput.buffer();
    float *end;

    float attack = mAttack.value();
    float release = mRelease.value();
    attack = MAX(0.5f * globalConfig.framePeriod, attack);
    release = MAX(0.5f * globalConfig.framePeriod, release);
    float slow = MAX(attack, release) * 3;

    // convert to coefficients
    // within 1% in attack/release secs
    attack = expf(logf(0.01f) / (attack * globalConfig.sampleRate));
    release = expf(logf(0.01f) / (release * globalConfig.sampleRate));
    slow = expf(logf(0.01f) / (slow * globalConfig.frameRate));

    float attack2 = 1.0f - attack;
    float release2 = 1.0f - release;
    float slow2 = 1.0f - slow;

    end = in + globalConfig.frameLength;
    float32x4_t sum = vdupq_n_f32(0.0f);
    while (in < end)
    {
      sum += vabsq_f32(vld1q_f32(in));
      in += 4;
    }

    float tmp[4];
    vst1q_f32(tmp, sum);
    mThreshold = slow2 * (tmp[0] + tmp[1] + tmp[2] + tmp[3]) / globalConfig.frameLength + slow * mThreshold;

    in = mInput.buffer();
    end = out + globalConfig.frameLength;
    while (out < end)
    {
      float x = fabs(*in);
      if (x > mThreshold)
      {
        mEnvelope = attack2 * x + attack * mEnvelope;
      }
      else
      {
        mEnvelope = release2 * x + release * mEnvelope;
      }
      *out = mEnvelope;
      in++;
      out++;
    }
  }

} /* namespace od */
