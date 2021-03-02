#include <core/objects/delays/Delay.h>
#include <od/extras/BigHeap.h>
#include <od/AudioThread.h>
#include <od/config.h>
#include <hal/ops.h>
#include <hal/simd.h>

namespace od
{

  Delay::Delay(int channelCount) : mChannelCount(channelCount)
  {
    addInput(mLeftInput);
    addParameter(mLeftDelay);
    addParameter(mSpread);
    addInput(mFeedback);
    addOutput(mLeftOutput);
    if (channelCount > 1)
    {
      addInput(mRightInput);
      addParameter(mRightDelay);
      addOutput(mRightOutput);
    }
    mFade.setLength(globalConfig.frameRate * 0.025f);
  }

  Delay::~Delay()
  {
    deallocate();
  }

  void Delay::zero()
  {
    if (mLeftDelayLine.mpBuffer)
    {
      bzero(mLeftDelayLine.mpBuffer, mMaxDelayInSamples * sizeof(float));
    }

    if (mRightDelayLine.mpBuffer)
    {
      bzero(mRightDelayLine.mpBuffer, mMaxDelayInSamples * sizeof(float));
    }
  }

  bool Delay::allocate(int Ns)
  {
    deallocate();
    int nbytes = Ns * sizeof(float);

    mLeftDelayLine.mpBuffer = BigHeap::allocateZeroed(nbytes);
    if (mLeftDelayLine.mpBuffer == 0)
    {
      return false;
    }

    if (mChannelCount > 1)
    {
      mRightDelayLine.mpBuffer = BigHeap::allocateZeroed(nbytes);
      if (mRightDelayLine.mpBuffer == 0)
      {
        deallocate();
        return false;
      }
    }

    return true;
  }

  void Delay::deallocate()
  {
    if (mLeftDelayLine.mpBuffer)
    {
      BigHeap::free(mLeftDelayLine.mpBuffer);
      mLeftDelayLine.mpBuffer = 0;
    }

    if (mRightDelayLine.mpBuffer)
    {
      BigHeap::free(mRightDelayLine.mpBuffer);
      mRightDelayLine.mpBuffer = 0;
    }
  }

  float Delay::allocateTimeUpTo(float seconds)
  {
    int Ns = globalConfig.sampleRate * MAX(0.001f, seconds);
    int Nf = (Ns / FRAMELENGTH + 1);
    Ns = Nf * FRAMELENGTH;
    if (Ns == mMaxDelayInSamples)
    {
      // Already allocated.
      return Ns * globalConfig.samplePeriod;
    }

    mMaxDelayInSamples = 0;
    mLeftDelayLine.mWriteIndex = 0;
    mLeftDelayLine.mReadIndex0 = 0;
    mLeftDelayLine.mReadIndex1 = 0;
    mRightDelayLine.mWriteIndex = 0;
    mRightDelayLine.mReadIndex0 = 0;
    mRightDelayLine.mReadIndex1 = 0;

    while (Nf > 1)
    {
      if (allocate(Nf * FRAMELENGTH))
      {
        mMaxDelayInSamples = Nf * FRAMELENGTH;
        return mMaxDelayInSamples * globalConfig.samplePeriod;
      }
      Nf /= 2;
    }

    // Failed to allocate memory
    return 0;
  }

  void Delay::process()
  {
    if (mMaxDelayInSamples == 0)
    {
      // Bypass when there is no memory allocated for the delay line(s).
      float *leftIn = mLeftInput.buffer();
      float *leftOut = mLeftOutput.buffer();
      memcpy(leftOut, leftIn, sizeof(float) * FRAMELENGTH);

      if (mChannelCount > 1)
      {
        float *rightIn = mRightInput.buffer();
        float *rightOut = mRightOutput.buffer();
        memcpy(rightOut, rightIn, sizeof(float) * FRAMELENGTH);
      }
      return;
    }

    if (mFade.done())
    {
      mFade.reset(1, 0);

      float spread = CLAMP(-1, 1, mSpread.value());
      float delayL = mLeftDelay.target() * (1 - spread);
      updateTime(mLeftDelayLine, delayL);
      if (mChannelCount > 1)
      {
        float delayR = mRightDelay.target() * (1 + spread);
        updateTime(mRightDelayLine, delayR);
      }
    }

    float *feedback = mFeedback.buffer();
    float *fade = AudioThread::getFrame();
    mFade.getInterpolatedFrame(fade);

    pushSamples(mLeftDelayLine, mLeftInput.buffer(), mLeftOutput.buffer(),
                feedback, fade);
    if (mChannelCount > 1)
    {
      pushSamples(mRightDelayLine, mRightInput.buffer(),
                  mRightOutput.buffer(), feedback, fade);
    }

    AudioThread::releaseFrame(fade);
    mFade.step();
  }

  void Delay::updateTime(InternalDelayLine &D, float delay)
  {
    D.mReadIndex0 = D.mReadIndex1;
    // Set the new read position based on the updated delay
    int n = delay * globalConfig.sampleRate;

    // Enforce multiples of 4 samples for Neon.
    n = (n / 4) * 4;
    if (n < 0)
      n = 0;
    else if (n > mMaxDelayInSamples)
      n = mMaxDelayInSamples;
    D.mReadIndex1 = D.mWriteIndex - n;
    if (D.mReadIndex1 < 0)
    {
      D.mReadIndex1 += mMaxDelayInSamples;
    }
  }

  void Delay::pushSamples(InternalDelayLine &D, float *in, float *out,
                          float *feedback, float *fade)
  {
    float *buffer = (float *)(D.mpBuffer);
    float32x4_t one = vdupq_n_f32(1);
    float32x4_t zero = vdupq_n_f32(0);

    for (int i = 0; i < FRAMELENGTH; i += 4)
    {
      float32x4_t x = vld1q_f32(in + i);

      // pre-write for zero-delay case
      vst1q_f32(buffer + D.mWriteIndex, x);

      // push 4 samples down the delay line
      float32x4_t y0 = vld1q_f32(buffer + D.mReadIndex0);
      float32x4_t y1 = vld1q_f32(buffer + D.mReadIndex1);
      float32x4_t w = vld1q_f32(fade + i);
      float32x4_t y = w * y0 + (one - w) * y1;
      float32x4_t g = vld1q_f32(feedback + i);
      // clamp feedback
      g = vminq_f32(one, vmaxq_f32(zero, g));
      vst1q_f32(buffer + D.mWriteIndex, vmlaq_f32(x, y, g));
      vst1q_f32(out + i, y);

      D.mReadIndex0 += 4;
      if (D.mReadIndex0 >= mMaxDelayInSamples)
      {
        D.mReadIndex0 = 0;
      }

      D.mReadIndex1 += 4;
      if (D.mReadIndex1 >= mMaxDelayInSamples)
      {
        D.mReadIndex1 = 0;
      }

      D.mWriteIndex += 4;
      if (D.mWriteIndex >= mMaxDelayInSamples)
      {
        D.mWriteIndex = 0;
      }
    }
  }

  float Delay::maximumDelayTime()
  {
    return mMaxDelayInSamples * globalConfig.samplePeriod;
  }

} /* namespace od */
