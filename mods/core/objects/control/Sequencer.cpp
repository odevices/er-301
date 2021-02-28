#include <core/objects/control/Sequencer.h>
#include <hal/simd.h>
#include <od/config.h>
#include <hal/ops.h>

#define LOCAL_USE_NEON 1

namespace od
{

  Sequencer::Sequencer()
  {
    addInput(mTrigger);
    addInput(mReset);
    addOutput(mOutput);
    addParameter(mStage);
    mStage.enableSerialization();
    addOption(mProcessingRate);
  }

  Sequencer::~Sequencer()
  {
  }

  void Sequencer::process()
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

  void Sequencer::atFrameRate()
  {
    bool trig = simd_any_positive(mTrigger.buffer(), FRAMELENGTH);
    bool reset = simd_any_positive(mReset.buffer(), FRAMELENGTH);
    int stage = MAX(0, mStage.roundValue());

    if (reset)
    {
      stage = 0;
    }

    if (trig)
    {
      stage++;
    }

    if (stage >= count())
    {
      stage = 0;
    }

    mStage.hardSet(stage);

    simd_set(mOutput.buffer(), FRAMELENGTH, get(stage));
  }

  void Sequencer::atSampleRate()
  {
    float *trig = mTrigger.buffer();
    float *reset = mReset.buffer();
    float *out = mOutput.buffer();
    int stage = MAX(0, mStage.roundValue());
    int N = count();

#if LOCAL_USE_NEON
    float32x4_t zero = vdupq_n_f32(0.0f);
    for (int i = 0; i < FRAMELENGTH; i += 4)
    {
      uint32_t TRIG[4], RESET[4];
      vst1q_u32(TRIG, vcgtq_f32(vld1q_f32(trig + i), zero));
      vst1q_u32(RESET, vcgtq_f32(vld1q_f32(reset + i), zero));
      for (int j = 0; j < 4; j++)
      {
        if (RESET[j])
        {
          stage = 0;
        }

        if (TRIG[j])
        {
          stage++;
        }

        if (stage >= N)
        {
          stage = 0;
        }

        out[i + j] = mStages[stage];
      }
    }
#else
    for (int i = 0; i < FRAMELENGTH; i++)
    {
      if (reset[i] > 0.0f)
      {
        stage = 0;
      }

      if (trig[i] > 0.0f)
      {
        stage++;
      }

      if (stage >= N)
      {
        stage = 0;
      }

      out[i] = mStages[stage];
    }
#endif

    mStage.hardSet(stage);
  }

  void Sequencer::clear()
  {
    mStages.clear();
  }

  void Sequencer::remove(int i)
  {
    if (i >= 0 && i < count())
    {
      mStages.erase(mStages.begin() + i);
    }
  }

  void Sequencer::insert(int i, float value)
  {
    if (i <= 0)
    {
      mStages.insert(mStages.begin(), value);
    }
    else if (i < count())
    {
      mStages.insert(mStages.begin() + i, value);
    }
    else
    {
      mStages.push_back(value);
    }
  }

  void Sequencer::push_back(float value)
  {
    mStages.push_back(value);
  }

  void Sequencer::push_front(float value)
  {
    mStages.insert(mStages.begin(), value);
  }

  int Sequencer::count()
  {
    return (int)mStages.size();
  }

  int Sequencer::get(int i)
  {
    if (i >= 0 && i < count())
    {
      return mStages[i];
    }
    else
    {
      return 0.0f;
    }
  }

  void Sequencer::set(int i, float value)
  {
    if (i >= 0 && i < count())
    {
      mStages[i] = value;
    }
  }

} /* namespace od */
