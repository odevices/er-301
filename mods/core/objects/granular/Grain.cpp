#include <core/objects/granular/Grain.h>
#include <od/extras/LookupTables.h>
#include <hal/simd.h>
#include <od/config.h>
#include <hal/ops.h>

namespace od
{

  Grain::Grain()
  {
  }

  Grain::~Grain()
  {
    setSample(0);
  }

  void Grain::setSample(Sample *sample)
  {
    stop();

    if (mpSample)
      mpSample->release();
    mpSample = sample;

    if (mpSample)
    {
      mpSample->attach();
      mSpeedAdjustment = sample->mSampleRate * globalConfig.samplePeriod;
    }
  }

  void Grain::init(int start, int duration, float speed, float gain, float pan)
  {
    mCurrentIndex = start;
    mDelayInSamples = 0;
    duration = MAX(64, duration);
    mDuration = mRemaining = 4 * (duration / 4);
    mPhase = 0.0f;
    mPhaseDelta = speed * mSpeedAdjustment;

    if (pan < -1e-5f)
    {
      mLeftBalance = gain;
      mRightBalance = gain * (1.0f + pan);
    }
    else if (pan > 1e-5f)
    {
      mLeftBalance = gain * (1.0f - pan);
      mRightBalance = gain;
    }
    else
    {
      mLeftBalance = gain;
      mRightBalance = gain;
    }

    mEnvelopePhase = 0.0f;
    mEnvelopePhaseDelta = 0.5f / mDuration;

    mActive = mpSample != 0;
  }

  void Grain::snapToZeroCrossing(int window)
  {
    int end;
    float cur, prev;

    int forward, backward;
    bool forwardFound = false, backwardFound = false;

    end = MIN(mpSample->mSampleCount, (uint32_t)(mCurrentIndex + window));
    prev = mpSample->get(mCurrentIndex, 0);
    for (int i = mCurrentIndex; i < end; i++)
    {
      cur = mpSample->get(i, 0);
      if (prev < 0 && cur >= 0)
      {
        forward = i;
        forwardFound = true;
        break;
      }
    }

    end = MAX(0, mCurrentIndex - window);
    prev = mpSample->get(mCurrentIndex, 0);
    for (int i = mCurrentIndex; i >= end; i--)
    {
      cur = mpSample->get(i, 0);
      if (cur <= 0 && prev > 0)
      {
        backward = i;
        backwardFound = true;
        break;
      }
    }

    // Choose the closest direction.
    if (forwardFound && backwardFound)
    {
      if (forward - mCurrentIndex < mCurrentIndex - backward)
      {
        mCurrentIndex = forward;
      }
      else
      {
        mCurrentIndex = backward;
      }
    }
    else if (backwardFound)
    {
      mCurrentIndex = backward;
    }
    else if (forwardFound)
    {
      mCurrentIndex = forward;
    }
  }

  void Grain::stop()
  {
    mRemaining = 0;
    mActive = false;
  }

  void Grain::generateSineWindow(float *out, int n)
  {
    float phase[4] = {0.0f, 0.0f, 0.0f, 0.0f};

    for (int i = 0; i < n; i += 4)
    {
      mEnvelopePhase += mEnvelopePhaseDelta;
      phase[0] = mEnvelopePhase;
      mEnvelopePhase += mEnvelopePhaseDelta;
      phase[1] = mEnvelopePhase;
      mEnvelopePhase += mEnvelopePhaseDelta;
      phase[2] = mEnvelopePhase;
      mEnvelopePhase += mEnvelopePhaseDelta;
      phase[3] = mEnvelopePhase;

      vst1q_f32(out + i, simd_sine_env(phase));
    }
  }

  void Grain::generateHanningWindow(float *out, int n)
  {
    float phase[4] = {0.0f, 0.0f, 0.0f, 0.0f};

    for (int i = 0; i < n; i += 4)
    {
      mEnvelopePhase += mEnvelopePhaseDelta;
      phase[0] = mEnvelopePhase;
      mEnvelopePhase += mEnvelopePhaseDelta;
      phase[1] = mEnvelopePhase;
      mEnvelopePhase += mEnvelopePhaseDelta;
      phase[2] = mEnvelopePhase;
      mEnvelopePhase += mEnvelopePhaseDelta;
      phase[3] = mEnvelopePhase;

      vst1q_f32(out + i, simd_hanning(phase));
    }
  }

  void Grain::generateTrapezoidWindow(float *out, int n)
  {
    float step = 1.0f / mFade;
    // Assuming: mRemaining <= n.
    for (int i = 0, d = mRemaining; i < n; i++, d--)
    {
      if (d < 0)
      {
        // finished
        out[i] = 0.0f;
      }
      else if (d < mFade)
      {
        // falling
        out[i] = d * step;
      }
      else if (d < mDuration - mFade)
      {
        // middle
        out[i] = 1.0f;
      }
      else
      {
        // rising
        out[i] = (mDuration - d) * step;
      }
    }
  }

  void Grain::generateEnvelope(float *out, int n)
  {
    switch (mEnvelopeType)
    {
    case mSineWindow:
      generateSineWindow(out, n);
      break;
    case mHanningWindow:
      generateHanningWindow(out, n);
      break;
    case mTrapezoidWindow:
      generateTrapezoidWindow(out, n);
      break;
    }

    if (mSquash > 1.0f)
    {
      squashEnvelope(out, n);
    }
  }

  void Grain::squashEnvelope(float *out, int n)
  {
    // x + c * x*x*x
    float32x4_t c = vdupq_n_f32(-1.0f / 6.75f);
    float32x4_t x, x3;
    float32x4_t max = vdupq_n_f32(1.5);
    float32x4_t min = vdupq_n_f32(-1.5);
    float32x4_t g = vdupq_n_f32(mSquash);

    for (int i = 0; i < n; i += 4)
    {
      x = g * vld1q_f32(out + i);
      x = vminq_f32(x, max);
      x = vmaxq_f32(x, min);
      x3 = vmulq_f32(x, vmulq_f32(x, x));
      vst1q_f32(out + i, vmlaq_f32(x, c, x3));
    }
  }

    void Grain::setEnvelope(int type)
    {
      mEnvelopeType = type;
      mFade = 0;
    }

    void Grain::setFade(int fade)
    {
      mFade = fade;
    }

    void Grain::setSquash(float squash)
    {
      mSquash = squash;
    }

    void Grain::setDelay(int samples)
    {
      // Positive delays only up to duration of grain.
      mDelayInSamples = CLAMP(0, mDuration, samples);
      // Enforce NEON alignment
      mDelayInSamples /= 4;
      mDelayInSamples *= 4;
    }

    int Grain::samplesRequired()
    {
      return mPhaseDelta * mRemaining;
    }  

} /* namespace od */
