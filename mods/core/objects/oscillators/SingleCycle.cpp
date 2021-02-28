#include <core/objects/oscillators/SingleCycle.h>
#include <hal/simd.h>
#include <math.h>

namespace od
{

  SingleCycle::SingleCycle()
  {
    addInput(mVoltPerOctave);
    addInput(mSync);
    addOutput(mOutput);
    addInput(mFundamental);
    addInput(mPhase);
    addParameter(mInternalPhase);
    mInternalPhase.enableSerialization();
  }

  SingleCycle::~SingleCycle()
  {
  }

  void SingleCycle::process()
  {
    if (mpSample == 0)
    {
      float *out = mOutput.buffer();
      simd_set(out, FRAMELENGTH, 0);
      return;
    }

    if (mpSlices)
    {
      int intervalCount = mpSlices->getIntervalCount();
      if (intervalCount == 0)
      {
        processOne(0, mpSample->mSampleCount);
      }
      else if (intervalCount == 1)
      {
        processOne(mpSlices->getIntervalStart(0),
                   mpSlices->getIntervalLength(0));
      }
      else
      {
        processMany();
      }
    }
    else
    {
      processOne(0, mpSample->mSampleCount);
    }
  }

  void SingleCycle::processOne(int offset, int length)
  {
    float *octave = mVoltPerOctave.buffer();
    float *out = mOutput.buffer();
    float *sync = mSync.buffer();
    float *freq = mFundamental.buffer();
    float *phase = mPhase.buffer();
    float32x4_t glog2 = vdupq_n_f32(FULLSCALE_IN_VOLTS * logf(2.0f));
    float32x4_t one = vdupq_n_f32(1.0f);
    float32x4_t zero = vdupq_n_f32(0.0f);
    float32x4_t negOne = vdupq_n_f32(-1.0f);
    float32x4_t sr = vdupq_n_f32(globalConfig.samplePeriod);
    float32x4_t N = vdupq_n_f32(length - 1);
    int32x4_t O = vdupq_n_s32(offset);
    float internalPhase = mInternalPhase.value();
    int32_t U[4] = {0};

    for (int i = 0; i < FRAMELENGTH; i += 4)
    {
      // calculate phase increment
      float32x4_t p = vld1q_f32(phase + i);
      float32x4_t dP = sr * vld1q_f32(freq + i);
      float32x4_t q = vld1q_f32(octave + i);
      q = vminq_f32(one, q);
      q = vmaxq_f32(negOne, q);
      q = dP * simd_exp(q * glog2);

      // accumulate phase while handling sync
      float tmp[4];
      uint32_t syncTrue[4];
      float32x4_t s = vld1q_f32(sync + i);
      vst1q_u32(syncTrue, vcgtq_f32(s, zero));
      vst1q_f32(tmp, q);
      for (int j = 0; j < 4; j++)
      {
        internalPhase += tmp[j];
        if (syncTrue[j])
        {
          internalPhase = 0;
        }
        tmp[j] = internalPhase;
      }
      q = vld1q_f32(tmp) + p;

      // wrap to [0,1]
      q = vsubq_f32(q, vcvtq_f32_s32(vcvtq_s32_f32(q)));
      q += one;
      q = vsubq_f32(q, vcvtq_f32_s32(vcvtq_s32_f32(q)));

      // look up
      float32x4_t pos = q * N;
      int32x4_t posI = vcvtq_s32_f32(pos);
      float32x4_t posX = vcvtq_f32_s32(posI);
      float32x4_t w1 = pos - posX;
      float32x4_t w0 = one - w1;

      float S0[4], S1[4];
      vst1q_s32(U, posI + O);
      for (int j = 0; j < 4; j++)
      {
        S0[j] = mpSample->get(U[j], 0);
        S1[j] = mpSample->get(U[j] + 1, 0);
      }
      float32x4_t s0 = vld1q_f32(S0);
      float32x4_t s1 = vld1q_f32(S1);
      vst1q_f32(out + i, w0 * s0 + w1 * s1);
    }

    internalPhase -= (int)internalPhase;
    mInternalPhase.hardSet(internalPhase);
    mCurrentIndex = U[3];
  }

  void SingleCycle::processMany()
  {
    float *octave = mVoltPerOctave.buffer();
    float *out = mOutput.buffer();
    float *sync = mSync.buffer();
    float *freq = mFundamental.buffer();
    float *phase = mPhase.buffer();
    float *select = mSliceSelect.buffer();
    float32x4_t glog2 = vdupq_n_f32(FULLSCALE_IN_VOLTS * logf(2.0f));
    float32x4_t one = vdupq_n_f32(1.0f);
    float32x4_t oneEp = vdupq_n_f32(0.9999f);
    float32x4_t zero = vdupq_n_f32(0.0f);
    float32x4_t negOne = vdupq_n_f32(-1.0f);
    float32x4_t sr = vdupq_n_f32(globalConfig.samplePeriod);
    float32x4_t M = vdupq_n_f32(mpSlices->getIntervalCount() - 1);
    float internalPhase = mInternalPhase.value();
    int32_t U[4], N0[4], N1[4];

    // Algorithm Outline (Naive wavetable with linear interpolation)
    // 1) Look up samples based on phase accumulator.
    // 2) Look up slices based on (continuous) select value.
    // 3) Calculate inter-sample interpolation weights.
    // 4) Calculate inter-slice interpolation weights.
    // 5) Do 4-way interpolation between the 4 samples at the corners.
    // Note: Assume slice lengths are different.

    for (int i = 0; i < FRAMELENGTH; i += 4)
    {
      // Calculate phase increment.
      float32x4_t p = vld1q_f32(phase + i);
      float32x4_t dP = sr * vld1q_f32(freq + i);
      float32x4_t q = vld1q_f32(octave + i);
      q = vminq_f32(one, q);
      q = vmaxq_f32(negOne, q);
      q = dP * simd_exp(q * glog2);

      // Accumulate phase while handling sync.
      float tmp[4];
      uint32_t syncTrue[4];
      float32x4_t s = vld1q_f32(sync + i);
      vst1q_u32(syncTrue, vcgtq_f32(s, zero));
      vst1q_f32(tmp, q);
      for (int j = 0; j < 4; j++)
      {
        internalPhase += tmp[j];
        if (syncTrue[j])
        {
          internalPhase = 0;
        }
        tmp[j] = internalPhase;
      }
      q = vld1q_f32(tmp) + p;

      // Wrap to [0,1]
      q = vsubq_f32(q, vcvtq_f32_s32(vcvtq_s32_f32(q)));
      q += one;
      q = vsubq_f32(q, vcvtq_f32_s32(vcvtq_s32_f32(q)));

      // Calculate inter-slice interpolation weights.
      float32x4_t t = vld1q_f32(select + i);
      t = vminq_f32(oneEp, t);
      t = vmaxq_f32(zero, t);
      float32x4_t m = t * M;
      int32x4_t mI = vcvtq_s32_f32(m);
      float32x4_t w1 = m - vcvtq_f32_s32(mI);
      float32x4_t w0 = one - w1;

      vst1q_s32(U, mI);
      for (int j = 0; j < 4; j++)
      {
        N0[j] = mpSlices->getIntervalLength(U[j]) - 1;
        N1[j] = mpSlices->getIntervalLength(U[j] + 1) - 1;
      }

      // Inter-sample weights for current slice.
      float32x4_t n0 = vcvtq_f32_s32(vld1q_s32(N0));
      float32x4_t pos0 = q * n0;
      int32x4_t posI0 = vcvtq_s32_f32(pos0);
      float32x4_t w01 = pos0 - vcvtq_f32_s32(posI0);
      float32x4_t w00 = one - w01;

      // Inter-sample weights for next slice.
      float32x4_t n1 = vcvtq_f32_s32(vld1q_s32(N1));
      float32x4_t pos1 = q * n1;
      int32x4_t posI1 = vcvtq_s32_f32(pos1);
      float32x4_t w11 = pos0 - vcvtq_f32_s32(posI1);
      float32x4_t w10 = one - w11;

      // Get the samples needed for the 4-way interpolation.
      float S00[4], S01[4], S10[4], S11[4];
      vst1q_s32(N0, posI0);
      vst1q_s32(N1, posI1);
      for (int j = 0, k; j < 4; j++)
      {
        k = N0[j] + mpSlices->getIntervalStart(U[j]);
        S00[j] = mpSample->get(k, 0);
        S01[j] = mpSample->get(k + 1, 0);
        k = N1[j] + mpSlices->getIntervalStart(U[j] + 1);
        S10[j] = mpSample->get(k, 0);
        S11[j] = mpSample->get(k + 1, 0);
      }

      // 4-way interpolation and store to the out buffer.
      float32x4_t s00 = vld1q_f32(S00);
      float32x4_t s01 = vld1q_f32(S01);
      float32x4_t s10 = vld1q_f32(S10);
      float32x4_t s11 = vld1q_f32(S11);
      vst1q_f32(out + i,
                w0 * (w00 * s00 + w01 * s01) + w1 * (w10 * s10 + w11 * s11));
    }

    internalPhase -= (int)internalPhase;
    mInternalPhase.hardSet(internalPhase);
    mCurrentIndex = N0[3] + mpSlices->getIntervalStart(U[3]);
    mActiveSliceIndex = U[3];
  }

} /* namespace od */
