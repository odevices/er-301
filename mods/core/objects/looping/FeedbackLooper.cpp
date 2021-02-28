#include <core/objects/looping/FeedbackLooper.h>
#include <od/config.h>
#include <hal/simd.h>
#include <hal/ops.h>
#include <string.h>

#define CURRENT(ch) (mpSample->get(mCurrentIndex, ch))
#define SHADOW(ch) (mpSample->get(mShadowIndex, ch))

#ifdef am335x
#define LOCAL_USE_NEON 1
#else
#define LOCAL_USE_NEON 0
#endif

namespace od
{

  FeedbackLooper::FeedbackLooper(int channelCount) : mChannelCount(channelCount)
  {
    addInput(mLeftInput);
    addOutput(mLeftOutput);
    addInput(mReset);
    addInput(mPunch);
    addInput(mEngage);
    addInput(mFeedback);
    addParameter(mResetPosition);
    addParameter(mFadeTime);

    int fadeTime = (int)(globalConfig.sampleRate * 0.01f);
    mPunchFade.setLength(fadeTime);
    mShadowFade.setLength(fadeTime);

    if (mChannelCount > 1)
    {
      addInput(mRightInput);
      addOutput(mRightOutput);
    }
  }

  FeedbackLooper::~FeedbackLooper()
  {
  }

  void FeedbackLooper::setSample(Sample *sample)
  {
    RecordHead::setSample(sample);
    float *left = mLeftOutput.buffer();
    float *right = mRightOutput.buffer();
    memset(left, 0, globalConfig.frameLength * sizeof(float));
    memset(right, 0, globalConfig.frameLength * sizeof(float));
    mShadowIndex = mCurrentIndex;
  }

  void FeedbackLooper::process()
  {
    if (mpSample == 0)
    {
      return;
    }

    if (mChannelCount < 2)
    {
      mono();
    }
    else
    {
      stereo();
    }
  }

  void FeedbackLooper::stereo()
  {
    float *leftIn = mLeftInput.buffer();
    float *rightIn = mRightInput.buffer();
    float *leftOut = mLeftOutput.buffer();
    float *rightOut = mRightOutput.buffer();
    float *trigger = mReset.buffer();
    float *punch = mPunch.buffer();
    float *engage = mEngage.buffer();
    float *feedback = mFeedback.buffer();
    bool dirty = mpSample->mDirty;
    int fadeTime = (int)(globalConfig.sampleRate * MAX(0.001f, mFadeTime.value()));

    mShadowFade.setLength(fadeTime);

    if (mResetRequested)
    {
      mResetRequested = false;
      setResetProportional(mResetPosition.value());
      jumpTo(mLeftResetIndex, fadeTime);
    }

    for (int i = 0; i < FRAMELENGTH; i++)
    {
      float sF = mShadowFade.step();

      if (punch[i] > 0 && !mPunched)
      {
        dirty = true;
        mPunched = true;
        mPunchFade.setLength(fadeTime);
        mPunchFade.reset(1);
      }
      else if (punch[i] == 0 && mPunched)
      {
        mPunched = false;
        mPunchFade.setLength(fadeTime);
        mPunchFade.reset(0);
      }

      if (mpSample->mChannelCount < 2)
      {
        // Processing for mono sample.
        if (engage[i] > 0.0f)
        {
          float x, original;
          float pF = mPunchFade.step();
          float in = 0.5f * (leftIn[i] + rightIn[i]);

          original = CURRENT(0);
          x = original * feedback[i] + in;  // dub
          x = (1 - pF) * original + pF * x; // punch
          x = sF * x + (1 - sF) * original; // shadow
          mpSample->set(mCurrentIndex, 0, x);

          original = SHADOW(0);
          x = original * feedback[i] + in;  // dub
          x = (1 - pF) * original + pF * x; // punch
          x = (1 - sF) * x + sF * original; // shadow
          mpSample->set(mShadowIndex, 0, x);
        }

        rightOut[i] = leftOut[i] = sF * CURRENT(0) + (1 - sF) * SHADOW(0);
      }
      else
      {
        if (engage[i] > 0.0f)
        {
          // Processing for stereo sample.
#if LOCAL_USE_NEON
          float pF = mPunchFade.step();
          float32x4_t f = vdupq_n_f32(feedback[i]);
          float32x4_t p1 = vdupq_n_f32(pF);
          float32x4_t p2 = vdupq_n_f32(1 - pF);

          float32x4_t orig = vdupq_n_f32(CURRENT(0));
          orig = vsetq_lane_f32(CURRENT(1), orig, 1);
          orig = vsetq_lane_f32(SHADOW(0), orig, 2);
          orig = vsetq_lane_f32(SHADOW(1), orig, 3);

          float32x4_t in = vdupq_n_f32(leftIn[i]);
          in = vsetq_lane_f32(rightIn[i], in, 1);
          in = vsetq_lane_f32(rightIn[i], in, 3);

          float32x4_t s1 = vdupq_n_f32(sF);
          s1 = vsetq_lane_f32(1 - sF, s1, 2);
          s1 = vsetq_lane_f32(1 - sF, s1, 3);

          float32x4_t s2 = vdupq_n_f32(1 - sF);
          s2 = vsetq_lane_f32(sF, s2, 2);
          s2 = vsetq_lane_f32(sF, s2, 3);

          float32x4_t x = orig * f + in;
          x = p1 * x + p2 * orig;
          x = s1 * x + s2 * orig;

          mpSample->set(mCurrentIndex, 0, vgetq_lane_f32(x, 0));
          mpSample->set(mCurrentIndex, 1, vgetq_lane_f32(x, 1));
          mpSample->set(mShadowIndex, 0, vgetq_lane_f32(x, 2));
          mpSample->set(mShadowIndex, 1, vgetq_lane_f32(x, 3));
#else
          float x, original;
          float pF = mPunchFade.step();

          original = CURRENT(0);
          x = original * feedback[i] + leftIn[i]; // dub
          x = (1 - pF) * original + pF * x;       // punch
          x = sF * x + (1 - sF) * original;       // shadow
          mpSample->set(mCurrentIndex, 0, x);

          original = CURRENT(1);
          x = original * feedback[i] + rightIn[i]; // dub
          x = (1 - pF) * original + pF * x;        // punch
          x = sF * x + (1 - sF) * original;        // shadow
          mpSample->set(mCurrentIndex, 1, x);

          original = SHADOW(0);
          x = original * feedback[i] + leftIn[i]; // dub
          x = (1 - pF) * original + pF * x;       // punch
          x = (1 - sF) * x + sF * original;       // shadow
          mpSample->set(mShadowIndex, 0, x);

          original = SHADOW(1);
          x = original * feedback[i] + rightIn[i]; // dub
          x = (1 - pF) * original + pF * x;        // punch
          x = (1 - sF) * x + sF * original;        // shadow
          mpSample->set(mShadowIndex, 1, x);
#endif
        }

        leftOut[i] = sF * CURRENT(0) + (1 - sF) * SHADOW(0);
        rightOut[i] = sF * CURRENT(1) + (1 - sF) * SHADOW(1);
      }

      if (!mPaused && engage[i] > 0.0f)
      {
        if (mCurrentIndex == mEndIndex)
        {
          mCurrentIndex = 0;
        }
        else
        {
          mCurrentIndex++;
        }

        if (mShadowIndex == mEndIndex)
        {
          mShadowIndex = 0;
        }
        else
        {
          mShadowIndex++;
        }
      }

      if (trigger[i] > 0.0f)
      {
        setResetProportional(mResetPosition.value());
        jumpTo(mLeftResetIndex, fadeTime);
      }
    }

    mpSample->mDirty = dirty;
  }

  void FeedbackLooper::mono()
  {

    float *in = mLeftInput.buffer();
    float *out = mLeftOutput.buffer();
    float *trigger = mReset.buffer();
    float *punch = mPunch.buffer();
    float *engage = mEngage.buffer();
    float *feedback = mFeedback.buffer();
    bool dirty = mpSample->mDirty;
    int fadeTime = (int)(globalConfig.sampleRate * MAX(0.001f, mFadeTime.value()));

    if (mResetRequested)
    {
      mResetRequested = false;
      setResetProportional(mResetPosition.value());
      jumpTo(mLeftResetIndex, fadeTime);
    }

    for (int i = 0; i < FRAMELENGTH; i++)
    {
      float sF = mShadowFade.step();

      if (punch[i] > 0 && !mPunched)
      {
        dirty = true;
        mPunched = true;
        mPunchFade.setLength(fadeTime);
        mPunchFade.reset(1);
      }
      else if (punch[i] == 0 && mPunched)
      {
        mPunched = false;
        mPunchFade.setLength(fadeTime);
        mPunchFade.reset(0);
      }

      if (mpSample->mChannelCount < 2)
      {
        // Processing for mono sample.
        if (engage[i] > 0.0f)
        {
          float x, original;
          float pF = mPunchFade.step();

          original = CURRENT(0);
          x = original * feedback[i] + in[i]; // dub
          x = (1 - pF) * original + pF * x;   // punch
          x = sF * x + (1 - sF) * original;   // shadow
          mpSample->set(mCurrentIndex, 0, x);

          original = SHADOW(0);
          x = original * feedback[i] + in[i]; // dub
          x = (1 - pF) * original + pF * x;   // punch
          x = (1 - sF) * x + sF * original;   // shadow
          mpSample->set(mShadowIndex, 0, x);
        }

        out[i] = sF * CURRENT(0) + (1 - sF) * SHADOW(0);
      }
      else
      {
        // Processing for stereo sample.
        if (engage[i] > 0.0f)
        {
          float x, original;
          float pF = mPunchFade.step();

          original = 0.5f * (CURRENT(0) + CURRENT(1));
          x = original * feedback[i] + in[i]; // dub
          x = (1 - pF) * original + pF * x;   // punch
          x = sF * x + (1 - sF) * original;   // shadow
          mpSample->set(mCurrentIndex, 0, x);
          mpSample->set(mCurrentIndex, 1, x);

          original = 0.5f * (SHADOW(0) + SHADOW(1));
          x = original * feedback[i] + in[i]; // dub
          x = (1 - pF) * original + pF * x;   // punch
          x = (1 - sF) * x + sF * original;   // shadow
          mpSample->set(mShadowIndex, 0, x);
          mpSample->set(mShadowIndex, 1, x);
        }

        out[i] = sF * CURRENT(0) + (1 - sF) * SHADOW(0);
        out[i] += sF * CURRENT(1) + (1 - sF) * SHADOW(1);
        out[i] *= 0.5f;
      }

      if (!mPaused && engage[i] > 0.0f)
      {
        if (mCurrentIndex == mEndIndex)
        {
          mCurrentIndex = 0;
        }
        else
        {
          mCurrentIndex++;
        }

        if (mShadowIndex == mEndIndex)
        {
          mShadowIndex = 0;
        }
        else
        {
          mShadowIndex++;
        }
      }

      if (trigger[i] > 0.0f)
      {
        setResetProportional(mResetPosition.value());
        jumpTo(mLeftResetIndex, fadeTime);
      }
    }

    mpSample->mDirty = dirty;
  }

} /* namespace od */
