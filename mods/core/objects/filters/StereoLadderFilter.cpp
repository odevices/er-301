/*
 * Inspired by: http://www.musicdsp.org/archive.php?classid=3#24
 */

#include <core/objects/filters/StereoLadderFilter.h>
#include <od/extras/LookupTables.h>
#include <od/extras/Random.h>
#include <od/AudioThread.h>
#include <hal/simd.h>
#include <od/config.h>
#include <hal/ops.h>
#include <math.h>

// band-limited saturation
#define SATURATE(x)                                   \
  {                                                   \
    x = vmin_f32(x, max);                             \
    x = vmax_f32(x, min);                             \
    x = vmla_f32(x, c9, vmul_f32(x, vmul_f32(x, x))); \
  }

namespace od
{

  StereoLadderFilter::StereoLadderFilter()
  {
    addInput(mLeftInput);
    addInput(mRightInput);
    addInput(mVoltPerOctave);
    addInput(mResonance);
    addOutput(mLeftOutput);
    addOutput(mRightOutput);
    addInput(mFundamental);
    mStage0 = vdup_n_f32(0);
    mStage1 = vdup_n_f32(0);
    mStage2 = vdup_n_f32(0);
    mStage3 = vdup_n_f32(0);
    mDelay0 = vdup_n_f32(0);
    mDelay1 = vdup_n_f32(0);
    mDelay2 = vdup_n_f32(0);
    mDelay3 = vdup_n_f32(0);
  }

  StereoLadderFilter::~StereoLadderFilter()
  {
  }

  // [Ladder Filter]: 1.2840% (34388 ticks, 373 Hz)
  void StereoLadderFilter::process()
  {
    float *leftOut = mLeftOutput.buffer();
    float *rightOut = mRightOutput.buffer();
    float *leftIn = mLeftInput.buffer();
    float *rightIn = mRightInput.buffer();
    float *octave = mVoltPerOctave.buffer();
    float *res = mResonance.buffer();
    float *freq = mFundamental.buffer();
    float minNormF = 2.0f * globalConfig.samplePeriod;

    float *P = AudioThread::getFrame();
    float *K = AudioThread::getFrame();
    float *R = AudioThread::getFrame();

    float32x4_t glog2 = vdupq_n_f32(FULLSCALE_IN_VOLTS * logf(2.0f));
    float32x4_t maxf = vdupq_n_f32(0.9999f);
    float32x4_t minf = vdupq_n_f32(minNormF);
    float32x4_t halfpi = vdupq_n_f32(0.5f * M_PI);
    float32x4_t sr = vdupq_n_f32(2.0f * globalConfig.samplePeriod);

    float32x4_t c1 = vdupq_n_f32(1.8f);
    float32x4_t c2 = vdupq_n_f32(-0.8f);
    float32x4_t c3 = vdupq_n_f32(2.0f);
    float32x4_t c4 = vdupq_n_f32(-1.0f);
    float32x4_t c6 = vdupq_n_f32(1.386249f);
    float32x4_t c7 = vdupq_n_f32(12.0f);
    float32x4_t c8 = vdupq_n_f32(6.0f);

    // calculate filter coefficients (sample-by-sample)
    for (int i = 0; i < FRAMELENGTH; i += 4)
    {
      float32x4_t q = vld1q_f32(octave + i);
      float32x4_t f0 = sr * vld1q_f32(freq + i);

      q = f0 * simd_exp(q * glog2);
      q = vminq_f32(maxf, q);
      q = vmaxq_f32(minf, q);
      float32x4_t cutoffq = q;

      q = simd_sin(halfpi * q);
      float32x4_t sq = q;

      // calc filter parameters
      float32x4_t pq = vmulq_f32(cutoffq, vmlaq_f32(c1, c2, cutoffq));
      //p = cutoff * (1.8f - 0.8f * cutoff);

      float32x4_t kq = vmlaq_f32(c4, c3, sq);
      //k = 2.0f * (*(ss++)) - 1.0f;
      float32x4_t t1q = vmlsq_f32(c6, c6, pq);
      //t1 = (1.0f - p) * 1.386249f;
      float32x4_t t2q = vmlaq_f32(c7, t1q, t1q);
      //t2 = 12.0f + t1 * t1;

      float32x4_t a = vmlaq_f32(t2q, c8, t1q);
      float32x4_t b = vmlsq_f32(t2q, c8, t1q);
      // division
      float32x4_t invb = vrecpeq_f32(b);
      // iterate 3 times for 24 bits of precision
      invb = vmulq_f32(invb, vrecpsq_f32(b, invb));
      invb = vmulq_f32(invb, vrecpsq_f32(b, invb));
      invb = vmulq_f32(invb, vrecpsq_f32(b, invb));

      float32x4_t rq = vmulq_f32(vld1q_f32(res + i), a * invb);
      //r = *(res++) * (t2 + 6.0f * t1) / (t2 - 6.0f * t1);

      vst1q_f32(R + i, rq);
      vst1q_f32(P + i, pq);
      vst1q_f32(K + i, kq);
    }

    float32x2_t c9 = vdup_n_f32(-1.0f / 6.0f);
    float32x2_t max = vdup_n_f32(1.4142135623730951f);
    float32x2_t min = vdup_n_f32(-1.4142135623730951f);
    float32x2_t noise = vdup_n_f32(Random::generateFloat(-1e-10f, 1e-10f));

    float32x2_t stage0 = mStage0;
    float32x2_t stage1 = mStage1;
    float32x2_t stage2 = mStage2;
    float32x2_t stage3 = mStage3;

    float32x2_t delay0 = mDelay0;
    float32x2_t delay1 = mDelay1;
    float32x2_t delay2 = mDelay2;
    float32x2_t delay3 = mDelay3;

    for (int i = 0; i < FRAMELENGTH; i++)
    {
      float32x2_t pd = vdup_n_f32(P[i]);
      float32x2_t kd = vdup_n_f32(K[i]);
      float32x2_t rd = vdup_n_f32(R[i]);

      float32x2_t xd;
      xd = vld1_lane_f32(leftIn + i, xd, 0);
      xd = vld1_lane_f32(rightIn + i, xd, 1);
      xd = vadd_f32(xd, noise);      // add regularization noise
      xd = vmls_f32(xd, rd, stage3); // feedback

      // Four cascaded one-pole filters (bilinear transform)
      // stage0 = pd * (xd + delay0) - kd * stage0;
      stage0 = vsub_f32(vmul_f32(pd, vadd_f32(xd, delay0)), vmul_f32(kd, stage0));
      // stage1 = pd * (stage0 + delay1) - kd * stage1;
      stage1 = vsub_f32(vmul_f32(pd, vadd_f32(stage0, delay1)), vmul_f32(kd, stage1));
      // stage2 = pd * (stage1 + delay2) - kd * stage2;
      stage2 = vsub_f32(vmul_f32(pd, vadd_f32(stage1, delay2)), vmul_f32(kd, stage2));
      // stage3 = pd * (stage2 + delay3) - kd * stage3;
      stage3 = vsub_f32(vmul_f32(pd, vadd_f32(stage2, delay3)), vmul_f32(kd, stage3));
      SATURATE(stage3);

      delay0 = xd;
      delay1 = stage0;
      delay2 = stage1;
      delay3 = stage2;

      vst1_lane_f32(leftOut + i, stage3, 0);
      vst1_lane_f32(rightOut + i, stage3, 1);
    }

    mStage0 = stage0;
    mStage1 = stage1;
    mStage2 = stage2;
    mStage3 = stage3;

    mDelay0 = delay0;
    mDelay1 = delay1;
    mDelay2 = delay2;
    mDelay3 = delay3;

    AudioThread::releaseFrame(P);
    AudioThread::releaseFrame(K);
    AudioThread::releaseFrame(R);
  }

} // namespace od
