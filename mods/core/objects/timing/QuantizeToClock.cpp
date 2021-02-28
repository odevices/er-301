/*
 * QuantizeToClock.cpp
 *
 *  Created on: 25 Jan 2018
 *      Author: clarkson
 */

#include <core/objects/timing/QuantizeToClock.h>
#include <od/config.h>
#include <hal/simd.h>

#define LOCAL_NEON_ENABLE 1

namespace od
{

  QuantizeToClock::QuantizeToClock()
  {
    addInput(mInput);
    addInput(mClock);
    addOutput(mOutput);
  }

  QuantizeToClock::~QuantizeToClock()
  {
  }

#if LOCAL_NEON_ENABLE
  // 8k cycles
  void QuantizeToClock::process()
  {
    float *in = mInput.buffer();
    float *clock = mClock.buffer();
    float *out = mOutput.buffer();
    float32x4_t thresh = vdupq_n_f32(0.5f);

    for (int i = 0; i < FRAMELENGTH; i += 4)
    {
      float32x4_t x = vld1q_f32(in + i);
      float32x4_t c = vld1q_f32(clock + i);
      uint32_t xHi[4], cHi[4];
      vst1q_u32(xHi, vcgtq_f32(x, thresh));
      vst1q_u32(cHi, vcgtq_f32(c, thresh));

      for (int j = 0; j < 4; j++)
      {
        if (!mInputState && xHi[j])
        {
          mGotRisingEdge = true;
          mInputState = true;
        }

        if (mInputState && !xHi[j])
        {
          mGotFallingEdge = true;
          mInputState = false;
        }

        if (!mClockState && cHi[j])
        {
          mClockState = true;
          if (mGotRisingEdge || mInputState)
          {
            mCurrentValue = 1.0f;
            mGotRisingEdge = false;
          }
        }

        if (mClockState && !cHi[j])
        {
          mClockState = false;
          if (mGotFallingEdge || !mInputState)
          {
            mCurrentValue = 0.0f;
            mGotFallingEdge = false;
          }
        }

        out[i + j] = mCurrentValue;
      }
    }
  }
#else
  // 15k cycles
  void QuantizeToClock::process()
  {
    float *in = mInput.buffer();
    float *clock = mClock.buffer();
    float *out = mOutput.buffer();

    for (int i = 0; i < FRAMELENGTH; i++)
    {
      if (!mInputState && in[i] > 0.5f)
      {
        mGotRisingEdge = true;
        mInputState = true;
      }

      if (mInputState && in[i] < 0.5f)
      {
        mGotFallingEdge = true;
        mInputState = false;
      }

      if (!mClockState && clock[i] > 0.5f)
      {
        mClockState = true;
        if (mGotRisingEdge || mInputState)
        {
          mCurrentValue = 1.0f;
          mGotRisingEdge = false;
        }
      }

      if (mClockState && clock[i] < 0.5f)
      {
        mClockState = false;
        if (mGotFallingEdge || !mInputState)
        {
          mCurrentValue = 0.0f;
          mGotFallingEdge = false;
        }
      }

      out[i] = mCurrentValue;
    }
  }
#endif

} // namespace od
/* namespace od */
