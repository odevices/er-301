/*
 * Comparator.cpp
 *
 *  Created on: Jun 11, 2016
 *      Author: clarkson
 */

#include <od/objects/timing/Comparator.h>
#include <hal/simd.h>

#define LOCAL_NEON_ENABLE 1

namespace od
{

  Comparator::Comparator()
  {
    addInput(mInput);
    addOutput(mOutput);
    addParameter(mThreshold);
    addParameter(mHysteresis);
    addOption(mState);
    mState.enableSerialization();
    addParameter(mLastEdgeCount);
    addParameter(mLastLevelCount);
    addOption(mMode);
  }

  Comparator::~Comparator()
  {
  }

  void Comparator::simulateRisingEdge()
  {
    if (mHeld)
    {
      mRisingEdgeHeld = !mRisingEdgeHeld;
    }
    else
    {
      mSimulateRisingEdge = true;
      mSecondsSinceLast = 0;
    }
  }

  void Comparator::simulateFallingEdge()
  {
    if (mHeld)
    {
      mFallingEdgeHeld = !mFallingEdgeHeld;
    }
    else
    {
      mSimulateFallingEdge = true;
    }
  }

  void Comparator::process()
  {

    if (mResetCounter)
    {
      mResetCounter = false;
      mRisingEdgeCount = 0;
      mLevelCount = 0;
      mSecondsSinceCounterReset = 0;
    }

    switch (mMode.value())
    {
    case COMPARATOR_TRIGGER_ON_RISE:
      processTriggerOnRise();
      break;
    case COMPARATOR_TRIGGER_ON_FALL:
      processTriggerOnFall();
      break;
    case COMPARATOR_TOGGLE:
      processToggle();
      break;
    case COMPARATOR_GATE:
    default:
      processGate();
      break;
    }

    if (mInvertOutput)
    {
      float *out = mOutput.buffer();
      for (int i = 0; i < FRAMELENGTH; i++)
      {
        out[i] = 1.0f - out[i];
      }
    }
  }

  void Comparator::processTriggerOnFall()
  {
    float *in = mInput.buffer();
    float *out = mOutput.buffer();
    int state = mState.value();
    int levelCount = 0;
    int edgeCount = 0;
    float threshold = mThreshold.value();
    float hys = mHysteresis.value();
    float risingThreshold = threshold + hys;
    float fallingThreshold = threshold - hys;

    if (!mSimulatedLevel)
    {
      simd_set(out, FRAMELENGTH, 0.0f);
      if (state == 0)
      {
        out[0] = 1.0f;
        state = 1;
        edgeCount++;
        levelCount++;
      }
    }
    else if (mInput.isConstant())
    {
      simd_set(out, FRAMELENGTH, 0.0f);

      if (state == 0 && in[0] < fallingThreshold)
      {
        out[0] = 1.0f;
        state = 1;
        edgeCount++;
        levelCount++;
      }
      else if (state == 1 && in[0] >= risingThreshold)
      {
        state = 0;
      }
    }
    else
    {
      float32x4_t rt = vdupq_n_f32(risingThreshold);
      float32x4_t ft = vdupq_n_f32(fallingThreshold);
      uint32_t above[4], below[4];

      for (int i = 0; i < FRAMELENGTH; i += 4)
      {
        float32x4_t x = vld1q_f32(in + i);
        vst1q_u32(above, vcgeq_f32(x, rt));
        vst1q_u32(below, vcltq_f32(x, ft));

        for (int j = 0; j < 4; j++)
        {
          if (state == 0 && below[j])
          {
            out[i + j] = 1;
            state = 1;
            edgeCount++;
            levelCount++;
          }
          else if (state == 1 && above[j])
          {
            state = 0;
            out[i + j] = 0;
          }
          else
          {
            out[i + j] = 0;
          }
        }
      }
    }

    // apply any simulated edges on the last sample of the frame
    if (mSimulateRisingEdge)
    {
      mSimulateRisingEdge = false;
      mSimulatedLevel = true;
    }
    else if (mSimulateFallingEdge)
    {
      mSimulateFallingEdge = false;
      mSimulatedLevel = false;
      // Reset the state, so that a trigger is generated on the next frame.
      state = 0;
    }

    mRisingEdgeCount += edgeCount;
    mLevelCount += levelCount;
    mLastEdgeCount.hardSet(edgeCount);
    mLastLevelCount.hardSet(levelCount);
    mSecondsSinceCounterReset += globalConfig.framePeriod;
    mState.set(state);
  }

  void Comparator::processTriggerOnRise()
  {
    float *in = mInput.buffer();
    float *out = mOutput.buffer();
    int state = mState.value();
    int levelCount = 0;
    int edgeCount = 0;
    float threshold = mThreshold.value();
    float hys = mHysteresis.value();
    float risingThreshold = threshold + hys;
    float fallingThreshold = threshold - hys;

    if (mSimulatedLevel)
    {
      simd_set(out, FRAMELENGTH, 0.0f);
      if (state == 0)
      {
        out[0] = 1.0f;
        state = 1;
        edgeCount++;
        levelCount++;
      }
    }
    else if (mInput.isConstant())
    {
      simd_set(out, FRAMELENGTH, 0.0f);

      if (state == 0 && in[0] >= risingThreshold)
      {
        out[0] = 1.0f;
        state = 1;
        edgeCount++;
        levelCount++;
      }
      else if (state == 1 && in[0] < fallingThreshold)
      {
        state = 0;
      }
    }
    else
    {

#if LOCAL_NEON_ENABLE
      float32x4_t rt = vdupq_n_f32(risingThreshold);
      float32x4_t ft = vdupq_n_f32(fallingThreshold);
      uint32_t above[4], below[4];

      for (int i = 0; i < FRAMELENGTH; i += 4)
      {
        float32x4_t x = vld1q_f32(in + i);
        vst1q_u32(above, vcgeq_f32(x, rt));
        vst1q_u32(below, vcltq_f32(x, ft));

        for (int j = 0; j < 4; j++)
        {
          if (state == 0 && above[j])
          {
            out[i + j] = 1;
            state = 1;
            edgeCount++;
            levelCount++;
          }
          else if (state == 1 && below[j])
          {
            state = 0;
            out[i + j] = 0;
          }
          else
          {
            out[i + j] = 0;
          }
        }
      }

#else
      for (int i = 0; i < FRAMELENGTH; i++)
      {
        if (state == 0 && in[i] >= risingThreshold)
        {
          out[i] = 1.0f;
          state = 1;
          edgeCount++;
          levelCount++;
        }
        else if (state == 1 && in[i] < fallingThreshold)
        {
          state = 0;
          out[i] = 0.0f;
        }
        else
        {
          out[i] = 0.0f;
        }
        out++;
        in++;
      }
#endif
    }

    // apply any simulated edges on the last sample of the frame
    if (mSimulateRisingEdge)
    {
      mSimulateRisingEdge = false;
      mSimulatedLevel = true;
      // Reset the state, so that a trigger is generated on the next frame.
      state = 0;
    }
    else if (mSimulateFallingEdge)
    {
      mSimulateFallingEdge = false;
      mSimulatedLevel = false;
    }

    mRisingEdgeCount += edgeCount;
    mLevelCount += levelCount;
    mLastEdgeCount.hardSet(edgeCount);
    mLastLevelCount.hardSet(levelCount);
    mSecondsSinceCounterReset += globalConfig.framePeriod;
    mState.set(state);
  }

  void Comparator::processToggle()
  {
    float *in = mInput.buffer();
    float *out = mOutput.buffer();
    int levelCount = 0;
    int edgeCount = 0;
    float threshold = mThreshold.value();
    float hys = mHysteresis.value();
    float risingThreshold = threshold + hys;
    float fallingThreshold = threshold - hys;
    int state = mState.value();

// toggle
#if LOCAL_NEON_ENABLE
    float32x4_t rt = vdupq_n_f32(risingThreshold);
    float32x4_t ft = vdupq_n_f32(fallingThreshold);
    uint32_t above[4], below[4];

    for (int i = 0; i < FRAMELENGTH; i += 4)
    {
      float32x4_t x = vld1q_f32(in + i);
      vst1q_u32(above, vcgeq_f32(x, rt));
      vst1q_u32(below, vcltq_f32(x, ft));

      for (int j = 0; j < 4; j++)
      {
        switch (state)
        {
        case 0:
          if (above[j])
          {
            state = 1;
            out[i + j] = 1.0f;
            edgeCount++;
            levelCount++;
          }
          else
          {
            out[i + j] = 0.0f;
          }
          break;
        case 1:
          if (below[j])
          {
            state = 2;
            out[i + j] = 1.0f;
            levelCount++;
          }
          else
          {
            out[i + j] = 1.0f;
            levelCount++;
          }
          break;
        case 2:
          if (above[j])
          {
            state = 3;
            out[i + j] = 0.0f;
          }
          else
          {
            out[i + j] = 1.0f;
            levelCount++;
          }
          break;
        case 3:
          if (below[j])
          {
            state = 0;
            out[i + j] = 0.0f;
          }
          else
          {
            out[i + j] = 0.0f;
          }
          break;
        }
      }
    }
#else
    for (int i = 0; i < FRAMELENGTH; i++)
    {
      switch (state)
      {
      case 0:
        if (in[i] >= risingThreshold)
        {
          state = 1;
          out[i] = 1.0f;
          edgeCount++;
          levelCount++;
        }
        else
        {
          out[i] = 0.0f;
        }
        break;
      case 1:
        if (in[i] < fallingThreshold)
        {
          state = 2;
          out[i] = 1.0f;
          levelCount++;
        }
        else
        {
          out[i] = 1.0f;
          levelCount++;
        }
        break;
      case 2:
        if (in[i] >= risingThreshold)
        {
          state = 3;
          out[i] = 0.0f;
        }
        else
        {
          out[i] = 1.0f;
          levelCount++;
        }
        break;
      case 3:
        if (in[i] < fallingThreshold)
        {
          state = 0;
          out[i] = 0.0f;
        }
        else
        {
          out[i] = 0.0f;
        }
        break;
      }
      out++;
      in++;
    }
#endif

    // apply any simulated edges on the last sample of the frame
    if (mSimulateRisingEdge)
    {
      mSimulateRisingEdge = false;

      if (state == 0)
      {
        state = 1;
        out[FRAMELENGTH - 1] = 1.0f;
        edgeCount++;
        levelCount++;
      }
      else if (state == 2)
      {
        state = 3;
        out[FRAMELENGTH - 1] = 0.0f;
      }
    }
    else if (mSimulateFallingEdge)
    {
      mSimulateFallingEdge = false;

      if (state == 1)
      {
        state = 2;
      }
      else if (state == 3)
      {
        state = 0;
      }
    }

    mRisingEdgeCount += edgeCount;
    mLevelCount += levelCount;
    mLastEdgeCount.hardSet(edgeCount);
    mLastLevelCount.hardSet(levelCount);
    mSecondsSinceCounterReset += globalConfig.framePeriod;
    mState.set(state);
  }

  void Comparator::processGate()
  {
    float *in = mInput.buffer();
    float *out = mOutput.buffer();
    int levelCount = 0;
    int edgeCount = 0;
    float threshold = mThreshold.value();
    float hys = mHysteresis.value();
    float risingThreshold = threshold + hys;
    float fallingThreshold = threshold - hys;
    int state = mState.value();

    // gate
    if (mSimulatedLevel)
    {
      if (state == 0)
      {
        state = 1;
        edgeCount++;
      }
      for (int i = 0; i < FRAMELENGTH; i++)
      {
        out[i] = 1;
      }
      levelCount += globalConfig.frameLength;
    }
    else
    {
#if LOCAL_NEON_ENABLE
      float32x4_t rt = vdupq_n_f32(risingThreshold);
      float32x4_t ft = vdupq_n_f32(fallingThreshold);
      uint32_t above[4], below[4];

      for (int i = 0; i < FRAMELENGTH; i += 4)
      {
        float32x4_t x = vld1q_f32(in + i);
        vst1q_u32(above, vcgeq_f32(x, rt));
        vst1q_u32(below, vcltq_f32(x, ft));

        for (int j = 0; j < 4; j++)
        {
          if (state)
          {
            if (below[j])
            {
              state = 0;
              out[i + j] = 0;
            }
            else
            {
              out[i + j] = 1;
              levelCount++;
            }
          }
          else
          {
            if (above[j])
            {
              state = 1;
              out[i + j] = 1;
              edgeCount++;
              levelCount++;
            }
            else
            {
              out[i + j] = 0;
            }
          }
        }
      }
#else
      while (out < end)
      {
        if (state)
        {
          if (in[i] < fallingThreshold)
          {
            state = 0;
            out[i] = 0.0f;
          }
          else
          {
            out[i] = 1.0f;
            levelCount++;
          }
        }
        else
        {
          if (in[i] >= risingThreshold)
          {
            state = 1;
            out[i] = 1.0f;
            edgeCount++;
            levelCount++;
          }
          else
          {
            out[i] = 0.0f;
          }
        }
        out++;
        in++;
      }
#endif
    }

    // apply any simulated edges on the last sample of the frame
    if (mSimulateRisingEdge)
    {
      mSimulateRisingEdge = false;
      mSimulatedLevel = true;
    }
    else if (mSimulateFallingEdge)
    {
      mSimulateFallingEdge = false;
      mSimulatedLevel = false;
    }

    mRisingEdgeCount += edgeCount;
    mLevelCount += levelCount;
    mLastEdgeCount.hardSet(edgeCount);
    mLastLevelCount.hardSet(levelCount);
    mSecondsSinceCounterReset += globalConfig.framePeriod;
    mState.set(state);
  }

  void Comparator::hold()
  {
    mHeld = true;
    mRisingEdgeHeld = false;
    mFallingEdgeHeld = false;
    mThreshold.hold();
  }

  void Comparator::unhold()
  {
    mThreshold.unhold();
    mSimulateRisingEdge = mRisingEdgeHeld;
    mSimulateFallingEdge = mFallingEdgeHeld;
    mHeld = false;
  }

} /* namespace od */
