#include <od/audio/SampleFifo.h>
#include <hal/ops.h>
#include <string.h>

namespace od
{

  SampleFifo::SampleFifo()
  {
    mBuffer.attach(); // and do not let go
    clear();
  }

  SampleFifo::~SampleFifo()
  {
    freeBuffer();
  }

  void SampleFifo::allocateBuffer(int Nc, int Ns)
  {
    mBuffer.allocateBuffer(Nc, 2 * Ns);
    mCapacity = Ns;
    mChannelCount = Nc;
    clear();
  }

  void SampleFifo::freeBuffer()
  {
    clear();
    mCapacity = 0;
    mChannelCount = 0;
    mBuffer.freeBuffer();
  }

  void SampleFifo::clear()
  {
    mHead = 0;
    mTail = 0;
    mSize = 0;
  }

  void SampleFifo::zero()
  {
    mBuffer.zero();
  }

  void SampleFifo::zeroAndFill()
  {
    mBuffer.zero();
    mHead = 0;
    mTail = 0;
    mSize = mCapacity;
  }

  void SampleFifo::pushMono(float *buffer, int n)
  {
    n = MIN(n, mCapacity - mSize);
    if (n == 0)
    {
      // FIFO is full.
      return;
    }

    if (mChannelCount == 1)
    {
      int biggest = mCapacity - mTail;
      if (biggest < n)
      {
        int m = n;
        copyMonoToMono(back(), buffer, biggest);
        copyMonoToMono(back2(), buffer, biggest);
        setTail(0);
        buffer += biggest;
        m -= biggest;
        copyMonoToMono(back(), buffer, m);
        copyMonoToMono(back2(), buffer, m);
        setTail(m);
      }
      else
      {
        copyMonoToMono(back(), buffer, n);
        copyMonoToMono(back2(), buffer, n);
        setTail(mTail + n);
      }
    }
    else
    {
      int biggest = mCapacity - mTail;
      if (biggest < n)
      {
        int m = n;
        copyMonoToStereo(back(), buffer, biggest);
        copyMonoToStereo(back2(), buffer, biggest);
        setTail(biggest);
        buffer += biggest;
        m -= biggest;
        copyMonoToStereo(back(), buffer, m);
        copyMonoToStereo(back2(), buffer, m);
        setTail(m);
      }
      else
      {
        copyMonoToStereo(back(), buffer, n);
        copyMonoToStereo(back2(), buffer, n);
        setTail(mTail + n);
      }
    }

    mSize += n;
  }

  void SampleFifo::pushStereo(float *buffer, int n)
  {
    n = MIN(n, mCapacity - mSize);
    if (n == 0)
    {
      // FIFO is full.
      return;
    }

    if (mChannelCount == 1)
    {
      int biggest = mCapacity - mTail;
      if (biggest < n)
      {
        int m = n;
        copyStereoToMono(back(), buffer, biggest);
        copyStereoToMono(back2(), buffer, biggest);
        setTail(0);
        buffer += biggest * 2;
        m -= biggest;
        copyStereoToMono(back(), buffer, m);
        copyStereoToMono(back2(), buffer, m);
        setTail(m);
      }
      else
      {
        copyStereoToMono(back(), buffer, n);
        copyStereoToMono(back2(), buffer, n);
        setTail(mTail + n);
      }
    }
    else
    {
      int biggest = mCapacity - mTail;
      if (biggest < n)
      {
        int m = n;
        copyStereoToStereo(back(), buffer, biggest);
        copyStereoToStereo(back2(), buffer, biggest);
        setTail(0);
        buffer += biggest * 2;
        m -= biggest;
        copyStereoToStereo(back(), buffer, m);
        copyStereoToStereo(back2(), buffer, m);
        setTail(m);
      }
      else
      {
        copyStereoToStereo(back(), buffer, n);
        copyStereoToStereo(back2(), buffer, n);
        setTail(mTail + n);
      }
    }

    mSize += n;
  }

  void SampleFifo::pop(int n)
  {
    int head = mHead;
    int size = mSize;
    n = MIN(n, size);
    head += n;
    if (head >= mCapacity)
    {
      head -= mCapacity;
    }
    mHead = head;
    mSize -= n;
  }

  void SampleFifo::popAll()
  {
    pop(mSize);
  }

  void SampleFifo::copyMonoToMono(float *dst, float *src, int n)
  {
    if (n > 0)
    {
      memcpy(dst, src, n * sizeof(float));
    }
  }

  void SampleFifo::copyStereoToStereo(float *dst, float *src, int n)
  {
    if (n > 0)
    {
      memcpy(dst, src, n * sizeof(float) * 2);
    }
  }

  void SampleFifo::copyStereoToMono(float *dst, float *src, int n)
  {
    for (int i = 0; i < n; i++)
    {
      dst[i] = 0.5f * (src[2 * i] + src[2 * i + 1]);
    }
  }

  void SampleFifo::copyMonoToStereo(float *dst, float *src, int n)
  {
    for (int i = 0; i < n; i++)
    {
      dst[2 * i] = dst[2 * i + 1] = src[i];
    }
  }

} /* namespace od */
