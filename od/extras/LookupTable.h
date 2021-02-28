/*
 * LookupTable.h
 *
 *  Created on: 9 Jun 2016
 *      Author: clarkson
 */

#ifndef APP_UTILS_LOOKUPTABLE_H_
#define APP_UTILS_LOOKUPTABLE_H_

#include <od/constants.h>
#include <vector>
#include <hal/simd.h>

namespace od
{

  class LookupTable
  {
  public:
    LookupTable();
    virtual ~LookupTable();

    inline float get(float phase)
    {
      if (phase < 0.0f)
      {
        return mValues.front();
      }
      else if (phase > 1.0f - EPSILON)
      {
        return mValues.back();
      }
      else
      {
        return mValues[(int)(phase * (mValues.size() - 1))];
      }
    }

    inline float getWrapped(float phase)
    {
      phase -= (int)phase;
      // [-1,1]
      phase++;
      // [0,2]
      phase -= (int)phase;
      // [0,1]
      return getNoCheck(phase);
    }

    inline float getWrappedInterpolated(float phase)
    {
      phase -= (int)phase;
      // [-1,1]
      phase++;
      // [0,2]
      phase -= (int)phase;
      // [0,1]
      return getNoCheckInterpolated(phase);
    }

    inline float getInterpolated(float phase)
    {
      if (phase < 0.0f)
      {
        return mValues.front();
      }
      else if (phase > 1.0f - EPSILON)
      {
        return mValues.back();
      }
      else
      {
        float pos = phase * (mValues.size() - 1);
        int i = (int)pos;
        float x = (float)i;
        float w1 = pos - x;
        float w0 = 1.0f - w1;
        return w0 * mValues[i] + w1 * mValues[i + 1];
      }
    }

    inline float getNoCheck(float phase)
    {
      return mValues[(int)(phase * (mValues.size() - 1))];
    }

    // assumes 0 to 1-epsilon phase
    inline float getNoCheckInterpolated(float phase)
    {
      float pos = phase * (mValues.size() - 1);
      int i = (int)pos;
      float x = (float)i;
      float w1 = pos - x;
      float w0 = 1.0f - w1;
      return w0 * mValues[i] + w1 * mValues[i + 1];
    }

    // assumes 0 to 1-epsilon phase
    inline float32x4_t getNoCheckInterpolated(float32x4_t phase)
    {
      float32x4_t pos = phase * vdupq_n_f32((mValues.size() - 1));
      uint32x4_t i = vcvtq_u32_f32(pos);
      float32x4_t x = vcvtq_f32_u32(i);
      float32x4_t w1 = pos - x;
      float32x4_t w0 = vdupq_n_f32(1.0f) - w1;
      uint32_t indices[4];
      vst1q_u32(indices, i);
      float a[4], b[4];
      a[0] = mValues[indices[0]];
      a[1] = mValues[indices[1]];
      a[2] = mValues[indices[2]];
      a[3] = mValues[indices[3]];
      b[0] = mValues[indices[0] + 1];
      b[1] = mValues[indices[1] + 1];
      b[2] = mValues[indices[2] + 1];
      b[3] = mValues[indices[3] + 1];
      return w0 * vld1q_f32(a) + w1 * vld1q_f32(b);
    }

    std::vector<float> mValues;
  };

} /* namespace od */

#endif /* APP_UTILS_LOOKUPTABLE_H_ */
