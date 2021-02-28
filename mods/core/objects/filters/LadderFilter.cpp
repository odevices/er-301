#include <core/objects/filters/LadderFilter.h>
#include <od/extras/LookupTables.h>
#include <od/AudioThread.h>
#include <hal/simd.h>
#include <od/config.h>
#include <hal/ops.h>
#include <math.h>

#define LADDERFILTER_USE_NEON 1

namespace od
{

  LadderFilter::LadderFilter()
  {
    addInput(mInput);
    addInput(mVoltPerOctave);
    addInput(mResonance);
    addOutput(mOutput);
    addParameter(mFundamental);
    memset(mStage, 0, sizeof(mStage));
    memset(mDelay, 0, sizeof(mDelay));
  }

  LadderFilter::~LadderFilter()
  {
    // TODO Auto-generated destructor stub
  }

#if LADDERFILTER_USE_NEON
  // [Ladder Filter]: 80k ticks
  void LadderFilter::process()
  {
    float *out = mOutput.buffer();
    float *in = mInput.buffer();
    float *octave = mVoltPerOctave.buffer();
    float *res = mResonance.buffer();
    float normF0 = 2.0f * MAX(1.0f, mFundamental.value()) * globalConfig.samplePeriod;
    float minNormF = 2.0f * globalConfig.samplePeriod;

    float *P = AudioThread::getFrame();
    float *K = AudioThread::getFrame();
    float *R = AudioThread::getFrame();

    float32x4_t glog2 = vdupq_n_f32(FULLSCALE_IN_VOLTS * logf(2.0f));
    float32x4_t maxf = vdupq_n_f32(0.9999f);
    float32x4_t minf = vdupq_n_f32(minNormF);
    float32x4_t f0 = vdupq_n_f32(normF0);
    float32x4_t halfpi = vdupq_n_f32(0.5f * M_PI);

    float32x4_t c1 = vdupq_n_f32(1.8f);
    float32x4_t c2 = vdupq_n_f32(-0.8f);
    float32x4_t c3 = vdupq_n_f32(2.0f);
    float32x4_t c4 = vdupq_n_f32(-1.0f);
    //float32x4_t c5 = vdupq_n_f32(1.0f);
    float32x4_t c5 = vdupq_n_f32(-1.386249f);
    float32x4_t c6 = vdupq_n_f32(1.386249f);
    float32x4_t c7 = vdupq_n_f32(12.0f);
    float32x4_t c8 = vdupq_n_f32(6.0f);
    float32x4_t c9 = vdupq_n_f32(-6.0f);

    for (int i = 0; i < FRAMELENGTH; i += 4)
    {
      float32x4_t q = vld1q_f32(octave + i);

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
      float32x4_t t1q = vmlaq_f32(c6, c5, pq);
      //t1 = (1.0f - p) * 1.386249f;
      float32x4_t t2q = vmlaq_f32(c7, t1q, t1q);
      //t2 = 12.0f + t1 * t1;

      float32x4_t a = vmlaq_f32(t2q, c8, t1q);
      float32x4_t b = vmlaq_f32(t2q, c9, t1q);
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

    double r, k, p, x;
    for (int i = 0; i < FRAMELENGTH; i++)
    {
      p = P[i];
      k = K[i];
      r = R[i];

      x = in[i] - r * mStage[3];

      // Four cascaded one-pole filters (bilinear transform)
      mStage[0] = p * (x + mDelay[0]) - k * mStage[0];
      mStage[1] = p * (mStage[0] + mDelay[1]) - k * mStage[1];
      mStage[2] = p * (mStage[1] + mDelay[2]) - k * mStage[2];
      mStage[3] = p * (mStage[2] + mDelay[3]) - k * mStage[3];

      // Clipping band-limited sigmoid
      //if(mStage[3] > 1.4142135623730951f) {
      //	mStage[3] = 1.4142135623730951f;
      //} else if(mStage[3] < -1.4142135623730951f) {
      //	mStage[3] = -1.4142135623730951f;
      //} else {
      //	mStage[3] -= (mStage[3] * mStage[3] * mStage[3]) / 6.0f;
      //}

      float y = MIN(1.4142135623730951f,
                    MAX(-1.4142135623730951f, mStage[3]));
      y -= (y * y * y) / 6.0f;
      mStage[3] = y;

      mDelay[0] = x;
      mDelay[1] = mStage[0];
      mDelay[2] = mStage[1];
      mDelay[3] = mStage[2];

      out[i] = mStage[3];
    }

    AudioThread::releaseFrame(P);
    AudioThread::releaseFrame(K);
    AudioThread::releaseFrame(R);
  }
#else
  // take 1: [LadderFilterUnit]: 9.3283% (124670 ticks, 748 Hz)
  void LadderFilter::process()
  {
    float *out = mOutput.buffer();
    float *end = out + globalConfig.frameLength;
    float *in = mInput.buffer();
    float *octave = mVoltPerOctave.buffer();
    float *res = mResonance.buffer();
    float normF0 = 2.0f * MAX(0.1f, mFundamental.value()) * globalConfig.samplePeriod;

    double cutoff, t1, t2, r, k, p, x;
    float fraction, factor, shifted;
    int twos;

    float *P = AudioThread::getFrame();
    float *K = AudioThread::getFrame();
    float *R = AudioThread::getFrame();
    float *end2 = res + globalConfig.frameLength;

    float *pp = P;
    float *kk = K;
    float *rr = R;

    while (res < end2)
    {
      // convert 1V/octave to factor
      shifted = *(octave++) + 10.0f;
      twos = (int)shifted;
      fraction = shifted - twos;
      factor = (1 << twos);
      factor *= 1.0f / (1 << 10);
      factor *= LookupTables::VoltPerOctave.getNoCheckInterpolated(fraction);
      cutoff = MIN(1.0f, normF0 * factor);

      // calc filter parameters
      p = cutoff * (1.8f - 0.8f * cutoff);
      k = 2.0f * LookupTables::Sine.getWrappedInterpolated(cutoff * 0.25f) - 1.0f;
      t1 = (1.0f - p) * 1.386249f;
      t2 = 12.0f + t1 * t1;
      r = *(res++) * (t2 + 6.0f * t1) / (t2 - 6.0f * t1);
      *(rr++) = r;
      *(pp++) = p;
      *(kk++) = k;
    }

    pp = P;
    kk = K;
    rr = R;

    while (out < end)
    {
      p = *(pp++);
      k = *(kk++);
      r = *(rr++);

      x = *(in++) - r * mStage[3];

      // Four cascaded one-pole filters (bilinear transform)
      mStage[0] = p * (x + mDelay[0]) - k * mStage[0];
      mStage[1] = p * (mStage[0] + mDelay[1]) - k * mStage[1];
      mStage[2] = p * (mStage[1] + mDelay[2]) - k * mStage[2];
      mStage[3] = p * (mStage[2] + mDelay[3]) - k * mStage[3];

      // Clipping band-limited sigmoid
      mStage[3] -= (mStage[3] * mStage[3] * mStage[3]) / 6.0f;

      mDelay[0] = x;
      mDelay[1] = mStage[0];
      mDelay[2] = mStage[1];
      mDelay[3] = mStage[2];

      *(out++) = mStage[3];
    }

    AudioThread::releaseFrame(P);
    AudioThread::releaseFrame(K);
    AudioThread::releaseFrame(R);
  }
#endif

} // namespace od
/* namespace od */
