#include <core/objects/control/BumpMap.h>
#include <od/extras/LookupTables.h>
#include <od/AudioThread.h>
#include <hal/ops.h>
#include <hal/simd.h>
#include <math.h>

namespace od
{

  BumpMap::BumpMap()
  {
    addInput(mInput);
    addInput(mPhase);
    addOutput(mOutput);
    addParameter(mCenter);
    addParameter(mWidth);
    addParameter(mHeight);
    addParameter(mFade);
    addOption(mInterpolation);
  }

  BumpMap::~BumpMap()
  {
    setSample(0, 0);
  }

  void BumpMap::setSample(Sample *sample, int channel)
  {
    Head::setSample(sample);
    if (sample)
    {
      mChannelIndex = CLAMP(0, (int)sample->mChannelCount - 1, channel);
    }
  }

  static inline float smoothstep(float x)
  {
    return x * x * (3 - 2 * x);
  }

  static inline float32x4_t neonSmoothstep(float32x4_t x)
  {
    return x * x * (3 - 2 * x);
  }

  void BumpMap::process()
  {
    if (mpSample)
    {
      processWithSample();
    }
    else
    {
      processWithNoSample();
    }
  }

  void BumpMap::processWithSample()
  {
    float *in = mInput.buffer();
    float *out = mOutput.buffer();
    float *phase = mPhase.buffer();
    float *tmp = AudioThread::getFrame();

    float width = CLAMP(0, 1, 0.5f * mWidth.value());
    float scale0 = mPreviousScale;
    float scale1 = 1.0f / width;

    float *ramp = LookupTables::FrameOfLinearRamp.mValues.data();

    float center1 = CLAMP(-1, 1, mCenter.value());
    float center0 = mPreviousCenter;

    float height1 = CLAMP(-1, 1, mHeight.value());
    float height0 = mPreviousHeight;

    float fade = CLAMP(0, 1, mFade.value());
    float edge0 = mPreviousEdge;
    float edge1 = CLAMP(0, 0.9999f, 1 - fade);

    if (width < 0.0001f)
    {
      // Width is basically zero, so flatten out the response.
      height1 = 0.0f;
      scale1 = 1 / 0.0001f;
    }

    if (fabs(center0 - center1) < 1e-8f && fabs(scale0 - scale1) < 1e-8f)
    {
      // first normalize
      for (int i = 0; i < FRAMELENGTH; i++)
      {
        out[i] = scale1 * (in[i] - center1);
      }
    }
    else
    {
      // first normalize
      for (int i = 0; i < FRAMELENGTH; i++)
      {
        float w1 = ramp[i];
        float w0 = 1.0f - w1;
        out[i] = in[i] - (w0 * center0 + w1 * center1);
        out[i] *= w0 * scale0 + w1 * scale1;
      }
    }

    int n = mpSample->mSampleCount;
    int k = 0;
    switch (mInterpolation.value())
    {
    case INTERPOLATION_NONE:
      for (int i = 0; i < FRAMELENGTH; i++)
      {
        float p = 0.5f * (out[i] + 1) + phase[i];
        p -= (int)p;
        // [-1,1]
        p++;
        // [0,2]
        p -= (int)p;
        // [0,1]
        float pos = p * (n - 1);
        k = (int)pos;
        tmp[i] = mpSample->get(k, mChannelIndex);
      }
      break;
    case INTERPOLATION_LINEAR:
    case INTERPOLATION_QUADRATIC:
      // interpolate values from the sample buffer
      for (int i = 0; i < FRAMELENGTH; i++)
      {
        float p = 0.5f * (out[i] + 1) + phase[i];
        p -= (int)p;
        // [-1,1]
        p++;
        // [0,2]
        p -= (int)p;
        // [0,1]
        float pos = p * (n - 1);
        k = (int)pos;
        float x = (float)k;
        float w1 = pos - x;
        float w0 = 1.0f - w1;
        tmp[i] = w0 * mpSample->get(k, mChannelIndex) + w1 * mpSample->get(k + 1, mChannelIndex);
      }
      break;
    }

    mCurrentIndex = k;

    if (fade < 0.0001f && width > 0.4999f)
    {
      for (int i = 0; i < FRAMELENGTH; i++)
      {
        out[i] = tmp[i] * height1;
      }
    }
    else
    {

      // then map using Smoothstep to fade
      // https://en.wikipedia.org/wiki/Smoothstep
      if (fabs(edge1 - edge0) < 1e-8f)
      {
        if (edge1 > 0.999f)
        {
          // no fade necessary
          for (int i = 0; i < FRAMELENGTH; i += 4)
          {
            float32x4_t x = vld1q_f32(out + i);
            vst1q_f32(out + i, simd_boxcar(x));
          }
        }
        else
        {
          float32x4_t z = vdupq_n_f32(1.0f / (1.0f - edge1));
          float32x4_t one = vdupq_n_f32(1.0f);
          float32x4_t neg1 = vdupq_n_f32(-1.0f);
          float32x4_t zero = vdupq_n_f32(0.0f);
          float32x4_t lower = vdupq_n_f32(-edge1);
          float32x4_t upper = vdupq_n_f32(edge1);
          for (int i = 0; i < FRAMELENGTH; i += 4)
          {
            float32x4_t x = vld1q_f32(out + i);
            float32x4_t fup = neonSmoothstep((x + one) * z);
            float32x4_t fdn = one - neonSmoothstep((x + lower) * z);

            uint32x4_t c0 = vcltq_f32(x, neg1);
            uint32x4_t c1 = vcltq_f32(x, lower);
            uint32x4_t c2 = vcltq_f32(x, upper);
            uint32x4_t c3 = vcltq_f32(x, one);

            float32x4_t y;
            y = vbslq_f32(vandq_u32(c1, vmvnq_u32(c0)), fup, zero);
            y = vbslq_f32(vandq_u32(c2, vmvnq_u32(c1)), one, y);
            y = vbslq_f32(vandq_u32(c3, vmvnq_u32(c2)), fdn, y);

            vst1q_f32(out + i, y);
          }
        }
      }
      else
      {
        for (int i = 0; i < FRAMELENGTH; i++)
        {
          float w1 = ramp[i];
          float w0 = 1.0f - w1;
          float edge = w0 * edge0 + w1 * edge1;

          if (out[i] < -1.0f)
          {
            out[i] = 0.0f;
          }
          else if (out[i] < -edge)
          {
            float z = 1.0f / (1.0f - edge);
            float x = (out[i] + 1.0f) * z;
            out[i] = smoothstep(x);
          }
          else if (out[i] < edge)
          {
            out[i] = 1.0f;
          }
          else if (out[i] < 1.0f)
          {
            float z = 1.0f / (1.0f - edge);
            float x = (out[i] - edge) * z;
            out[i] = 1.0f - smoothstep(x);
          }
          else
          {
            out[i] = 0.0f;
          }
        }
      }

      // then scale by height and mapped sample
      if (fabs(height0 - height1) < 1e-8f)
      {
        for (int i = 0; i < FRAMELENGTH; i++)
        {
          out[i] *= height1 * tmp[i];
        }
      }
      else
      {
        for (int i = 0; i < FRAMELENGTH; i++)
        {
          float w1 = ramp[i];
          float w0 = 1.0f - w1;
          out[i] *= tmp[i] * (w0 * height0 + w1 * height1);
        }
      }
    }

    mPreviousCenter = center1;
    mPreviousScale = scale1;
    mPreviousHeight = height1;
    mPreviousEdge = edge1;
    AudioThread::releaseFrame(tmp);
  }

  void BumpMap::processWithNoSample()
  {
    float *in = mInput.buffer();
    float *out = mOutput.buffer();

    float width = CLAMP(0, 1, 0.5f * mWidth.value());
    float scale0 = mPreviousScale;
    float scale1 = 1.0f / width;

    float *ramp = LookupTables::FrameOfLinearRamp.mValues.data();

    float center1 = CLAMP(-1, 1, mCenter.value());
    float center0 = mPreviousCenter;

    float height1 = CLAMP(-1, 1, mHeight.value());
    float height0 = mPreviousHeight;

    float fade = CLAMP(0, 1, mFade.value());
    float edge0 = mPreviousEdge;
    float edge1 = CLAMP(0, 0.9999f, 1 - fade);

    if (width < 0.0001f)
    {
      // Width is basically zero, so flatten out the response.
      height1 = 0.0f;
      scale1 = 1 / 0.0001f;
    }

    if (fade < 0.0001f && width > 0.4999f)
    {
      for (int i = 0; i < FRAMELENGTH; i++)
      {
        out[i] = height1;
      }
    }
    else
    {

      if (fabs(center0 - center1) < 1e-8f && fabs(scale0 - scale1) < 1e-8f)
      {
        // first normalize
        for (int i = 0; i < FRAMELENGTH; i++)
        {
          out[i] = scale1 * (in[i] - center1);
        }
      }
      else
      {
        // first normalize
        for (int i = 0; i < FRAMELENGTH; i++)
        {
          float w1 = ramp[i];
          float w0 = 1.0f - w1;
          out[i] = in[i] - (w0 * center0 + w1 * center1);
          out[i] *= w0 * scale0 + w1 * scale1;
        }
      }

      // then map using Smoothstep to fade
      // https://en.wikipedia.org/wiki/Smoothstep
      if (fabs(edge1 - edge0) < 1e-8f)
      {
        if (edge1 > 0.999f)
        {
          // no fade necessary
#if 1
          for (int i = 0; i < FRAMELENGTH; i += 4)
          {
            float32x4_t x = vld1q_f32(out + i);
            vst1q_f32(out + i, simd_boxcar(x));
          }
#else
          for (int i = 0; i < FRAMELENGTH; i++)
          {
            if (out[i] < -1)
            {
              out[i] = 0;
            }
            else if (out[i] < 1)
            {
              out[i] = 1;
            }
            else
            {
              out[i] = 0;
            }
          }
#endif
        }
        else
        {
#if 1
          float32x4_t z = vdupq_n_f32(1.0f / (1.0f - edge1));
          float32x4_t one = vdupq_n_f32(1.0f);
          float32x4_t neg1 = vdupq_n_f32(-1.0f);
          float32x4_t zero = vdupq_n_f32(0.0f);
          float32x4_t lower = vdupq_n_f32(-edge1);
          float32x4_t upper = vdupq_n_f32(edge1);
          for (int i = 0; i < FRAMELENGTH; i += 4)
          {
            float32x4_t x = vld1q_f32(out + i);
            float32x4_t fup = neonSmoothstep((x + one) * z);
            float32x4_t fdn = one - neonSmoothstep((x + lower) * z);

            uint32x4_t c0 = vcltq_f32(x, neg1);
            uint32x4_t c1 = vcltq_f32(x, lower);
            uint32x4_t c2 = vcltq_f32(x, upper);
            uint32x4_t c3 = vcltq_f32(x, one);

            float32x4_t y;
            y = vbslq_f32(vandq_u32(c1, vmvnq_u32(c0)), fup, zero);
            y = vbslq_f32(vandq_u32(c2, vmvnq_u32(c1)), one, y);
            y = vbslq_f32(vandq_u32(c3, vmvnq_u32(c2)), fdn, y);

            vst1q_f32(out + i, y);
          }
#else
          float z = 1.0f / (1.0f - edge1);
          for (int i = 0; i < FRAMELENGTH; i++)
          {
            if (out[i] < -1.0f)
            {
              out[i] = 0.0f;
            }
            else if (out[i] < -edge1)
            {
              float x = (out[i] + 1.0f) * z;
              out[i] = smoothstep(x);
            }
            else if (out[i] < edge1)
            {
              out[i] = 1.0f;
            }
            else if (out[i] < 1.0f)
            {
              float x = (out[i] - edge1) * z;
              out[i] = 1.0f - smoothstep(x);
            }
            else
            {
              out[i] = 0.0f;
            }
          }
#endif
        }
      }
      else
      {
        for (int i = 0; i < FRAMELENGTH; i++)
        {
          float w1 = ramp[i];
          float w0 = 1.0f - w1;
          float edge = w0 * edge0 + w1 * edge1;

          if (out[i] < -1.0f)
          {
            out[i] = 0.0f;
          }
          else if (out[i] < -edge)
          {
            float z = 1.0f / (1.0f - edge);
            float x = (out[i] + 1.0f) * z;
            out[i] = smoothstep(x);
          }
          else if (out[i] < edge)
          {
            out[i] = 1.0f;
          }
          else if (out[i] < 1.0f)
          {
            float z = 1.0f / (1.0f - edge);
            float x = (out[i] - edge) * z;
            out[i] = 1.0f - smoothstep(x);
          }
          else
          {
            out[i] = 0.0f;
          }
        }
      }

      // then scale by height
      if (fabs(height0 - height1) < 1e-8f)
      {
        for (int i = 0; i < FRAMELENGTH; i++)
        {
          out[i] *= height1;
        }
      }
      else
      {
        for (int i = 0; i < FRAMELENGTH; i++)
        {
          float w1 = ramp[i];
          float w0 = 1.0f - w1;
          out[i] *= w0 * height0 + w1 * height1;
        }
      }
    }

    mPreviousCenter = center1;
    mPreviousScale = scale1;
    mPreviousHeight = height1;
    mPreviousEdge = edge1;
  }

} /* namespace od */
