#include <od/audio/Sample.h>
#include <od/audio/WavFileReader.h>
#include <od/audio/WavFileWriter.h>
#include <od/extras/Conversions.h>
#include <od/extras/BigHeap.h>
#include <od/extras/Random.h>
#include <od/constants.h>
#include <hal/ops.h>
#include <hal/simd.h>
#include <string.h>

namespace od
{

  Sample::Sample()
  {
  }

  Sample::Sample(float sampleRate, uint32_t Nc, uint32_t Ns) : mSampleRate(sampleRate)
  {
    allocateBuffer(Nc, Ns, true);
  }

  Sample::~Sample()
  {
    freeBuffer();
  }

  void Sample::zero()
  {
    if (mpData)
    {
      memset(mpData, 0, getSizeInBytes());
      alterWaterMark();
    }
  }

  void Sample::setBuffer(float *buffer, uint32_t Nc, uint32_t Ns)
  {
    freeBuffer();

    if (buffer)
    {
      mpData = buffer;
      mChannelCount = Nc;
      mSampleCount = Ns;
      setSampleRate(mSampleRate);
      alterWaterMark();
    }
  }

  bool Sample::allocateBuffer(uint32_t _Nc, uint32_t _Ns, bool zeroed)
  {
    freeBuffer();

    int sizeInBytes = sizeof(float) * _Ns * _Nc;

    if (zeroed)
    {
      mpData = (float *)BigHeap::allocateZeroed(sizeInBytes);
    }
    else
    {
      mpData = (float *)BigHeap::allocate(sizeInBytes);
    }

    if (mpData)
    {
      mChannelCount = _Nc;
      mSampleCount = _Ns;
      setSampleRate(mSampleRate);
      alterWaterMark();
      return true;
    }
    else
    {
      return false;
    }
  }

  int Sample::prepareForLoading(SampleLoadInfo &info)
  {
    if (info.mChannelCount == 0 || info.mSampleCount == 0 || info.mEntries.size() == 0)
    {
      return STATUS_ERROR_OPENING_FILE;
    }

    if (!allocateBuffer(info.mChannelCount, info.mSampleCount, false))
    {
      return STATUS_OUT_OF_MEMORY;
    }

    setSampleRate(info.mEntries[0].sampleRate);
    mOriginalBitDepth = info.mEntries[0].bitDepth;

    return STATUS_PREPARED;
  }

  void Sample::freeBuffer()
  {
    if (mpData)
    {
      BigHeap::free((char *)mpData);
    }

    mpData = NULL;
    mSampleCount = 0;
    mSampleLoadCount = 0;
    mChannelCount = 0;
  }

  void Sample::setSampleRate(float rate)
  {
    mSampleRate = rate;
    mSamplePeriod = 1.0f / rate;
    mTotalSeconds = (double)mSampleCount * (double)mSamplePeriod;
    mMinutes = mTotalSeconds / 60;
    mSeconds = mTotalSeconds - mMinutes * 60;
    mMilliseconds = 1000 * (mTotalSeconds - mMinutes * 60 - mSeconds);
  }

  bool Sample::saveWavFile(const char *filename, WavFileEncoding encoding)
  {
    WavFileWriter writer(mSampleRate, mChannelCount, encoding);

    if (!writer.open(filename))
    {
      return false;
    }

    if (writer.writeSamples(mpData, mSampleCount) != mSampleCount)
    {
      return false;
    }

    return writer.close();
  }

  bool Sample::loadWavFile(const char *filename, bool verbose)
  {
    WavFileReader reader;

    if (!reader.open(filename))
    {
      return false;
    }

    if (verbose)
    {
      reader.describe();
    }

    if (!allocateBuffer(reader.getChannelCount(), reader.getSampleCount(),
                        false))
    {
      return false;
    }

    setSampleRate(reader.getSampleRate());
    mOriginalBitDepth = reader.getBitDepth();

    if (reader.readSamples(mpData, reader.getSampleCount()) != reader.getSampleCount())
    {
      freeBuffer();
      return false;
    }

    mSampleLoadCount = mSampleCount;

    return true;
  }

  void Sample::normalize(int from, int to, float targetAmplitude)
  {
    if (mpData)
    {
      from = MAX(0, from);
      to = MIN((int)mSampleCount, to);

      if (from >= to)
        return;

      // find maximum value
      float maxAmplitude = 0.0f;
      for (int i = from; i < to; i++)
      {
        for (int j = 0; j < (int)mChannelCount; j++)
        {
          maxAmplitude = MAX(maxAmplitude, fabs(get(i, j)));
        }
      }

      if (maxAmplitude < fromDecibels(-96))
      {
        // too small, abort
        return;
      }

      float scale = targetAmplitude / maxAmplitude;
      int n = mSampleCount * mChannelCount;
      for (int i = 0; i < n; i++)
      {
        mpData[i] *= scale;
      }

      mDirty = true;
      alterWaterMark();
    }
  }

  void Sample::copyFrom(int from, int to, Sample *sample, int sourceStart)
  {
    if (mpData && sample)
    {
      sourceStart = MAX(0, sourceStart);
      from = MAX(0, from);
      to = MIN((int)mSampleCount, to);

      if (from >= to)
        return;

      if (sourceStart >= (int)sample->mSampleCount)
      {
        return;
      }

      int available = sample->mSampleCount - sourceStart;
      to = MIN(to, from + available);

      int channelCount = MIN(mChannelCount, sample->mChannelCount);

      int i;
      for (i = from; i < to; i++)
      {
        for (int j = 0; j < channelCount; j++)
        {
          set(i, j, sample->get(i + sourceStart - from, j));
        }
      }

      mDirty = true;
      alterWaterMark();
    }
  }

  void Sample::silence(int from, int to)
  {
    if (mpData)
    {
      from = MAX(0, from);
      to = MIN((int)mSampleCount, to);

      if (from >= to)
        return;

      memset(mpData + from * mChannelCount, 0,
             (to - from) * mChannelCount * sizeof(float));

      mDirty = true;
      alterWaterMark();
    }
  }

  void Sample::fadeIn(int from, int to)
  {
    if (mpData)
    {
      from = MAX(0, from);
      to = MIN((int)mSampleCount, to);

      if (from >= to)
        return;

      double step = 1.0 / (to - from);
      double gain = 0.0;
      for (int i = from; i < to; i++)
      {
        for (int j = 0; j < (int)mChannelCount; j++)
        {
          set(i, j, get(i, j) * gain);
        }
        gain += step;
      }

      mDirty = true;
      alterWaterMark();
    }
  }

  void Sample::fadeOut(int from, int to)
  {
    if (mpData)
    {
      from = MAX(0, from);
      to = MIN((int)mSampleCount, to);

      if (from >= to)
        return;

      double step = 1.0 / (to - from);
      double gain = 1.0;
      for (int i = from; i < to; i++)
      {
        for (int j = 0; j < (int)mChannelCount; j++)
        {
          set(i, j, get(i, j) * gain);
        }
        gain -= step;
      }

      mDirty = true;
      alterWaterMark();
    }
  }

  void Sample::removeDC(int from, int to)
  {
    if (mpData)
    {
      from = MAX(0, from);
      to = MIN((int)mSampleCount, to);

      if (from >= to)
        return;

      // Kahan summation algorithm
      // https://en.wikipedia.org/wiki/Kahan_summation_algorithm
      std::vector<float> sums(mChannelCount, 0.0f);
      std::vector<float> compensation(mChannelCount, 0.0f);
      for (int i = from; i < to; i++)
      {
        for (int j = 0; j < (int)mChannelCount; j++)
        {
          float y = get(i, j) - compensation[j];
          float t = sums[j] + y;
          compensation[j] = (t - sums[j]) - y;
          sums[j] = t;
        }
      }

      float scale = 1.0f / (to - from);
      for (int j = 0; j < (int)mChannelCount; j++)
      {
        sums[j] *= scale;
      }

      for (int i = from; i < to; i++)
      {
        for (int j = 0; j < (int)mChannelCount; j++)
        {
          set(i, j, get(i, j) - sums[j]);
        }
      }

      mDirty = true;
      alterWaterMark();
    }
  }

  void Sample::alterWaterMark()
  {
    mWaterMark = Random::generateUnsignedInteger();
  }

} /* namespace od */
