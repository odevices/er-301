/*
 * FastEWMA.cpp
 *
 *  Created on: 4 Sep 2016
 *      Author: clarkson
 */

#include <od/extras/FastEWMA.h>
#include <hal/simd.h>
#include <hal/simd.h>

namespace od
{

  FastEWMA::FastEWMA()
  {
    // TODO Auto-generated constructor stub
  }

  FastEWMA::~FastEWMA()
  {
    // TODO Auto-generated destructor stub
  }

  float FastEWMA::push(float *buffer, int n)
  {
    float *end = buffer + n;
    float32x4_t alpha = vdupq_n_f32(mAlpha);
    float32x4_t omega = vdupq_n_f32(1.0f - mAlpha);
    float32x4_t y = vdupq_n_f32(mY);
    float32x4_t x;

    while (buffer < end)
    {
      x = vld1q_f32(buffer);
      y = vaddq_f32(vmulq_f32(alpha, x), vmulq_f32(omega, y));
      buffer += 4;
    }

    float tmp[4];
    vst1q_f32(tmp, y);
    mY = 0.25f * (tmp[0] + tmp[1] + tmp[2] + tmp[3]);
    return mY;
  }

  float FastEWMA::pushMagnitudes(float *buffer, int n)
  {
    float *end = buffer + n;
    float32x4_t alpha = vdupq_n_f32(mAlpha);
    float32x4_t omega = vdupq_n_f32(1.0f - mAlpha);
    float32x4_t y = vdupq_n_f32(mY);
    float32x4_t x;

    while (buffer < end)
    {
      x = vabsq_f32(vld1q_f32(buffer));
      y = vaddq_f32(vmulq_f32(alpha, x), vmulq_f32(omega, y));
      buffer += 4;
    }

    float tmp[4];
    vst1q_f32(tmp, y);
    mY = 0.25f * (tmp[0] + tmp[1] + tmp[2] + tmp[3]);
    return mY;
  }

} /* namespace od */
