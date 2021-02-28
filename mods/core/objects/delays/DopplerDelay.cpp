#include <core/objects/delays/DopplerDelay.h>
#include <hal/simd.h>
#include <od/extras/BigHeap.h>
#include <od/config.h>
#include <hal/ops.h>
#include <string.h>

namespace od
{

  DopplerDelay::DopplerDelay(float secs)
  {
    addInput(mInput);
    addInput(mDelay);
    addInput(mFeedback);
    addOutput(mOutput);
    allocateTimeUpTo(secs);
  }

  DopplerDelay::~DopplerDelay()
  {
    deallocate();
  }

  void DopplerDelay::zero()
  {
    if (mpBuffer)
    {
      bzero(mpBuffer, mMaxDelayInSamples * sizeof(float));
    }
  }

  bool DopplerDelay::allocate(int Ns)
  {
    deallocate();
    int nbytes = Ns * sizeof(float);

    mpBuffer = (float *)BigHeap::allocateZeroed(nbytes);
    if (mpBuffer == 0)
    {
      return false;
    }

    return true;
  }

  void DopplerDelay::deallocate()
  {
    if (mpBuffer)
    {
      BigHeap::free((char *)mpBuffer);
      mpBuffer = 0;
    }
  }

  float DopplerDelay::allocateTimeUpTo(float seconds)
  {
    int Ns = globalConfig.sampleRate * seconds;
    int Nf = (Ns / FRAMELENGTH + 1);
    Ns = Nf * FRAMELENGTH;
    if (Ns == mMaxDelayInSamples)
    {
      // Already allocated.
      return Ns * globalConfig.samplePeriod;
    }

    mMaxDelayInSamples = 0;
    mpBegin = 0;
    mpEnd = 0;
    mpWrite = 0;
    mpRead = 0;

    while (Nf > 1)
    {
      if (allocate(Nf * FRAMELENGTH))
      {
        mMaxDelayInSamples = Nf * FRAMELENGTH;
        mMaxDelayInSeconds = mMaxDelayInSamples * globalConfig.samplePeriod;
        mpBegin = mpBuffer;
        mpEnd = mpBuffer + mMaxDelayInSamples - 1;
        mpWrite = mpBegin;
        mpRead = mpBegin;
        return mMaxDelayInSeconds;
      }
      Nf /= 2;
    }

    // Failed to allocate memory
    return 0;
  }

  static inline int sampleDistance(float *small, float *big)
  {
    ptrdiff_t d = big - small;
    return d;
  }

  inline void DopplerDelay::read()
  {
    int k = (int)mPhase;
    mPhase -= k;

    while (k > 3)
    {
      int x = k - 3;
      float *p = mpRead + x;
      if (p > mpEnd)
      {
        // passing through mpEnd
        k -= sampleDistance(mpRead, mpEnd);
        k--;
        jumpTo(mpBegin);
      }
      else
      {
        k -= x;
        mpRead = p;
        break;
      }
    }

    while (k < -3)
    {
      int x = k + 3;
      float *p = mpRead + x;
      if (p < mpBegin)
      {
        // passing through mpStart
        k += sampleDistance(mpBegin, mpRead);
        k++;
        jumpTo(mpEnd);
      }
      else
      {
        k -= x;
        mpRead = p;
        break;
      }
    }

    // going backwards
    while (k < 0)
    {
      k++;
      // consume next sample
      mFifo[2] = mFifo[1];
      mFifo[1] = mFifo[0];
      mFifo[0] = mpRead[0];

      if (mpRead == mpBegin)
      {
        jumpTo(mpEnd);
      }
      else
      {
        mpRead--;
      }
    }

    // going forwards
    while (k > 0)
    {
      k--;
      // consume next sample
      mFifo[2] = mFifo[1];
      mFifo[1] = mFifo[0];
      mFifo[0] = mpRead[0];

      if (mpRead == mpEnd)
      {
        jumpTo(mpBegin);
      }
      else
      {
        mpRead++;
      }
    }
  }

  inline void DopplerDelay::write(float *in, float *out, float *feedback,
                                  float *phase, float *recent0,
                                  float *recent1, float *recent2)
  {
    float32x4_t x = vld1q_f32(in);
    float32x4_t y = simd_quadratic_interpolate_with_return(recent0, recent1,
                                                           recent2, phase);
    float32x4_t g = vld1q_f32(feedback);
    g = vminq_f32(vdupq_n_f32(1.0f), vmaxq_f32(vdupq_n_f32(0.0f), g));
    vst1q_f32(mpWrite, vmlaq_f32(x, y, g));
    vst1q_f32(out, y);

    mpWrite += 4;
    if (mpWrite > mpEnd)
    {
      mpWrite = mpBegin;
    }
  }

  inline void DopplerDelay::read4(float *phaseDelta, float *phase,
                                  float *recent0, float *recent1,
                                  float *recent2)
  {
    mPhase += phaseDelta[0];
    read();
    phase[0] = mPhase;
    recent0[0] = mFifo[0];
    recent1[0] = mFifo[1];
    recent2[0] = mFifo[2];

    mPhase += phaseDelta[1];
    read();
    phase[1] = mPhase;
    recent0[1] = mFifo[0];
    recent1[1] = mFifo[1];
    recent2[1] = mFifo[2];

    mPhase += phaseDelta[2];
    read();
    phase[2] = mPhase;
    recent0[2] = mFifo[0];
    recent1[2] = mFifo[1];
    recent2[2] = mFifo[2];

    mPhase += phaseDelta[3];
    read();
    phase[3] = mPhase;
    recent0[3] = mFifo[0];
    recent1[3] = mFifo[1];
    recent2[3] = mFifo[2];
  }

  void DopplerDelay::process()
  {
    float *in = mInput.buffer();
    float *delay = mDelay.buffer();
    float *out = mOutput.buffer();
    float *feedback = mFeedback.buffer();
    float tmp[4];

    if (mMaxDelayInSamples == 0)
    {
      // Bypass when there is no memory allocated for the delay line.
      memcpy(out, in, sizeof(float) * FRAMELENGTH);
      return;
    }

    float32x4_t maxDelay = vdupq_n_f32(mMaxDelayInSeconds);
    float32x4_t sr = vdupq_n_f32((float)globalConfig.sampleRate);
    float32x4_t one = vdupq_n_f32(1.0f);
    float32x4_t d0, d1;
    // neon state for interpolation
    float phase[4] = {0.0f, 0.0f, 0.0f, 0.0f};
    float recent0[4] = {0.0f, 0.0f, 0.0f, 0.0f};
    float recent1[4] = {0.0f, 0.0f, 0.0f, 0.0f};
    float recent2[4] = {0.0f, 0.0f, 0.0f, 0.0f};

    // handle frame boundary for first iteration

    // pre-write for zero-delay case
    vst1q_f32(mpWrite, vld1q_f32(in));

    tmp[0] = mPreviousDelay;
    tmp[1] = delay[0];
    tmp[2] = delay[1];
    tmp[3] = delay[2];
    d0 = vminq_f32(maxDelay, vld1q_f32(tmp));
    d1 = vminq_f32(maxDelay, vld1q_f32(delay));
    // tmp = 1 - sr * (change in delay)
    vst1q_f32(tmp, one + sr * (d0 - d1));

    read4(tmp, phase, recent0, recent1, recent2);
    write(in, out, feedback, phase, recent0, recent1, recent2);

    // remaining iterations...
    for (int i = 4; i < FRAMELENGTH; i += 4)
    {
      // pre-write for zero-delay case
      vst1q_f32(mpWrite, vld1q_f32(in + i));

      d0 = vminq_f32(maxDelay, vld1q_f32(delay + i - 1));
      d1 = vminq_f32(maxDelay, vld1q_f32(delay + i));
      // tmp = 1 - sr * (change in delay)
      vst1q_f32(tmp, one + sr * (d0 - d1));

      read4(tmp, phase, recent0, recent1, recent2);
      write(in + i, out + i, feedback + i, phase, recent0, recent1, recent2);
    }

    // Save the last delay for when we process the next frame.
    mPreviousDelay = delay[FRAMELENGTH - 1];
  }

  float DopplerDelay::maximumDelayTime()
  {
    return mMaxDelayInSeconds;
  }

} /* namespace od */
