/*
 * TrackAndHold.cpp
 *
 *  Created on: 7 Jan 2017
 *      Author: clarkson
 */

#include <core/objects/timing/TrackAndHold.h>
#include <hal/simd.h>
#include <od/config.h>

#define LOCAL_NEON_ENABLE 1

namespace od
{

  TrackAndHold::TrackAndHold()
  {
    addInput(mInput);
    addInput(mTrack);
    addOutput(mOutput);
    addOption(mFlavor);
    addParameter(mValue);
    mValue.enableSerialization();
    mValue.deserializeWithHardSet();
  }

  TrackAndHold::~TrackAndHold()
  {
  }

#if LOCAL_NEON_ENABLE
  void TrackAndHold::process()
  {
    float value = mValue.value();
    float *in = mInput.buffer();
    float *track = mTrack.buffer();
    float *out = mOutput.buffer();
    float32x4_t thresh = vdupq_n_f32(0.5f);

    switch (mFlavor.value())
    {
    case TRACKNHOLD_CHOICE_HIGH:
      for (int i = 0; i < FRAMELENGTH; i += 4)
      {
        float32x4_t x = vld1q_f32(track + i);
        uint32_t high[4];
        vst1q_u32(high, vcgtq_f32(x, thresh));
        for (int j = 0; j < 4; j++)
        {
          if (high[j])
          {
            value = in[i + j];
          }
          out[i + j] = value;
        }
      }
      break;
    case TRACKNHOLD_CHOICE_LOW:
      for (int i = 0; i < FRAMELENGTH; i += 4)
      {
        float32x4_t x = vld1q_f32(track + i);
        uint32_t low[4];
        vst1q_u32(low, vcltq_f32(x, thresh));
        for (int j = 0; j < 4; j++)
        {
          if (low[j])
          {
            value = in[i + j];
          }
          out[i + j] = value;
        }
      }
      break;
    case TRACKNHOLD_CHOICE_MINMAX:
      for (int i = 0; i < FRAMELENGTH; i++)
      {
        if (track[i] < 0.5f && in[i] < value)
        {
          value = in[i];
        }
        else if (track[i] > 0.5f && in[i] > value)
        {
          value = in[i];
        }
        out[i] = value;
      }
      break;
    }

    mValue.hardSet(value);
  }
#else
  void TrackAndHold::process()
  {
    float *in = mInput.buffer();
    float *track = mTrack.buffer();
    float *out = mOutput.buffer();

    switch (mFlavor.value())
    {
    case TRACKNHOLD_CHOICE_HIGH:
      for (int i = 0; i < FRAMELENGTH; i++)
      {
        if (track[i] > 0.5f)
        {
          mValue = in[i];
        }
        out[i] = mValue;
      }
      break;
    case TRACKNHOLD_CHOICE_LOW:
      for (int i = 0; i < FRAMELENGTH; i++)
      {
        if (track[i] < 0.5f)
        {
          mValue = in[i];
        }
        out[i] = mValue;
      }
      break;
    case TRACKNHOLD_CHOICE_MINMAX:
      for (int i = 0; i < FRAMELENGTH; i++)
      {
        if (track[i] < 0.5f && in[i] < mValue)
        {
          mValue = in[i];
        }
        else if (track[i] > 0.5f && in[i] > mValue)
        {
          mValue = in[i];
        }
        out[i] = mValue;
      }
      break;
    }
  }
#endif
} // namespace od
/* namespace od */
