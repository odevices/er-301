#include <core/objects/Counter.h>
#include <od/objects/Inlet.h>
#include <od/objects/Option.h>
#include <od/objects/Parameter.h>
#include <od/config.h>
#include <hal/simd.h>

#define LOCAL_USE_NEON 1

namespace od
{

  Counter::Counter()
  {
    addInput(mInput);
    addInput(mReset);
    addOutput(mOutput);
    addParameter(mStepSize);
    addParameter(mStart);
    addParameter(mFinish);
    addParameter(mGain);
    addParameter(mBias);
    addParameter(mValue);
    mValue.enableSerialization();
    addOption(mWrap);
    addOption(mProcessingRate);
  }

  Counter::~Counter()
  {
  }

  void Counter::process()
  {
    if (mProcessingRate.value() == PER_FRAME)
    {
      atFrameRate();
    }
    else
    {
      atSampleRate();
    }
  }

  void Counter::atFrameRate()
  {
    // Generate output first.
    float gain = mGain.value();
    float bias = mBias.value();
    int value = mValue.roundTarget();
    simd_set(mOutput.buffer(), FRAMELENGTH, value * gain + bias);

    // Update internal value.
    bool in = simd_any_positive(mInput.buffer(), FRAMELENGTH);
    bool reset = simd_any_positive(mReset.buffer(), FRAMELENGTH);
    int start = mStart.roundTarget();
    int finish = mFinish.roundTarget();
    int step = mStepSize.roundTarget();

    int onFinish, onStart, onReset;

    if (mWrap.value() == CHOICE_YES)
    {
      onFinish = start;
      onStart = finish;
    }
    else
    {
      onFinish = finish;
      onStart = start;
    }

    if (step > 0)
    {
      onReset = start;
    }
    else
    {
      onReset = finish;
    }

    if (in)
    {
      value += step;
      if (value > finish)
      {
        value = onFinish;
      }
      else if (value < start)
      {
        value = onStart;
      }
    }

    if (reset)
    {
      value = onReset;
    }

    mValue.hardSet(value);
  }

  void Counter::atSampleRate()
  {
    float *in = mInput.buffer();
    float *out = mOutput.buffer();
    float *reset = mReset.buffer();
    int start = mStart.roundTarget();
    int finish = mFinish.roundTarget();
    int step = mStepSize.roundTarget();
    float gain = mGain.value();
    int value = mValue.roundTarget();
    int onFinish, onStart, onReset;

    if (mWrap.value() == CHOICE_YES)
    {
      onFinish = start;
      onStart = finish;
    }
    else
    {
      onFinish = finish;
      onStart = start;
    }

    if (step > 0)
    {
      onReset = start;
    }
    else
    {
      onReset = finish;
    }

#if LOCAL_USE_NEON
    float32x4_t zero = vdupq_n_f32(0.0f);
    float32x4_t G = vdupq_n_f32(gain);

    for (int i = 0; i < FRAMELENGTH; i += 4)
    {
      uint32_t IN[4], RESET[4];
      float V[4];
      vst1q_u32(IN, vcgtq_f32(vld1q_f32(in + i), zero));
      vst1q_u32(RESET, vcgtq_f32(vld1q_f32(reset + i), zero));

      for (int j = 0; j < 4; j++)
      {
        if (IN[j])
        {
          value += step;
          if (value > finish)
          {
            value = onFinish;
          }
          else if (value < start)
          {
            value = onStart;
          }
        }

        if (RESET[j])
        {
          value = onReset;
        }

        V[j] = value;
      }

      vst1q_f32(out + i, G * vld1q_f32(V));
    }
#else
    for (int i = 0; i < FRAMELENGTH; i++)
    {
      if (in[i] > 0.0f)
      {
        value += step;
        if (value > finish)
        {
          value = onFinish;
        }
        else if (value < start)
        {
          value = onStart;
        }
      }

      if (reset[i] > 0.0f)
      {
        value = onReset;
      }

      out[i] = value * gain;
    }
#endif

    mValue.hardSet(value);
  }

} /* namespace od */
