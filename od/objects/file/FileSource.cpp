#include <od/objects/file/FileSource.h>
#include <od/extras/LookupTables.h>
#include <od/UIThread.h>
#include <od/config.h>
#include <string.h>

#define READAHEAD_IN_FRAMES (16 * 10)
#define READCHUNK_IN_FRAMES 16

namespace od
{

  FileSource::FileSource(int channelCount) : mOutputChannelCount(channelCount), mReadJob(this)
  {
    own(mReadJob); // and don't let go
    if (channelCount == 1)
    {
      addOutput(mOutput);
    }
    else if (channelCount == 2)
    {
      addOutput(mLeftOutput);
      addOutput(mRightOutput);
    }
    addParameter(mSpeed);
    addParameter(mPosition);
    addParameter(mReset);
    addOption(mLoop);
  }

  FileSource::~FileSource()
  {
    stop();
    close();
  }

  void FileSource::start()
  {
    if (!mRunning)
    {
      mRunning = true;
    }
  }

  void FileSource::stop()
  {
    if (mRunning)
    {
      mRunning = false;
      mReadJob.wait();
    }
  }

  bool FileSource::open(const std::string &filename)
  {
    close();

    mFilename = filename;

    if (mReader.open(mFilename))
    {
      mError = false;
      mErrorMessage = "";
      mEOF = false;
      mInputChannelCount = mReader.getChannelCount();
      mSamplePeriod = 1.0f / mReader.getSampleRate();
      if (mInputChannelCount == 2 && mOutputChannelCount == 2)
      {
        mStereoResampler.setInputRate(mReader.getSampleRate());
        mFifo.allocateBuffer(2,
                             READAHEAD_IN_FRAMES * mStereoResampler.required(1.0f));
      }
      else
      {
        mMonoResampler.setInputRate(mReader.getSampleRate());
        mFifo.allocateBuffer(1,
                             READAHEAD_IN_FRAMES * mMonoResampler.required(1.0f));
      }
      mFifo.zero();
      mSamplesPerChunk = READCHUNK_IN_FRAMES * globalConfig.frameLength;
      mChunkBuffer.allocateBuffer(mInputChannelCount, mSamplesPerChunk);
      start();
      return true;
    }
    else
    {
      mError = true;
      mErrorMessage = "Failed to open.";
      mSamplePeriod = 0.0f;
      return false;
    }
  }

  void FileSource::close()
  {
    stop();
    if (mReader.mIsOpen)
    {
      mReader.close();
      mFifo.freeBuffer();
      mChunkBuffer.freeBuffer();
      mEOF = true;
    }
  }

  void FileSource::setPositionInSamples(uint32_t position)
  {
    mRequestedPosition = position;
    mHavePendingSeek = true;
  }

  float FileSource::getMaxBufferDepthInSeconds()
  {
    return mFifo.capacity() * mSamplePeriod;
  }

  float FileSource::getBufferDepthInSeconds()
  {
    return mFifo.size() * mSamplePeriod;
  }

  void FileSource::readWork()
  {
    if (mError || !mReader.mIsOpen)
      return;

    if (mReader.getSampleCount() == 0)
    {
      mError = true;
      mErrorMessage = "No samples.";
      return;
    }

    if (mHavePendingSeek)
    {
      mHavePendingSeek = false;
      mReader.seekSamples(mRequestedPosition);
    }

    int n = mSamplesPerChunk;
    if (!isLooping() && mReader.tellSamples() + n > mReader.getSampleCount())
    {
      n = mReader.getSampleCount() - mReader.tellSamples();
    }

    if (n > 0)
    {
      if (readSamples(mChunkBuffer.mpData, n))
      {
        if (mInputChannelCount == 1)
        {
          mFifo.pushMono(mChunkBuffer.mpData, n);
        }
        else
        {
          mFifo.pushStereo(mChunkBuffer.mpData, n);
        }
      }
      else
      {
        mError = true;
        mErrorMessage = "Read failed.";
      }
    }
    else
    {
      mEOF = true;
    }
  }

  bool FileSource::readSamples(float *buffer, int n)
  {
    int total = (int)mReader.getSampleCount();
    int need = n;
    float *ptr = buffer;
    while (need > 0)
    {
      int cur = (int)mReader.tellSamples();
      int remaining = total - cur;
      int target;
      if (remaining == 0)
      {
        if (isLooping())
        {
          mReader.seekSamples(0);
          remaining = total;
        }
        else
        {
          memset(ptr, 0, need * sizeof(float) * mInputChannelCount);
          mEOF = true;
          return true;
        }
      }
      else
      {
        mEOF = false;
      }

      target = MIN(remaining, need);

      int actual = (int)mReader.readSamples(ptr, target);
      if (actual != target)
      {
        mEOF = true;
        return false;
      }

      ptr += actual * mInputChannelCount;
      need -= actual;
    }

    return true;
  }

  void FileSource::process()
  {
    float speed = mPaused ? 0.0f : mSpeed.value();
    bool doReset = mReset.value() > 0.0f;

    if (doReset)
    {
      setPositionInSamples(mPosition.value() * getDurationInSamples());
    }

    int have = mFifo.size();
    if (mOutputChannelCount == 1)
    {
      float *out = mOutput.buffer();

      // How many input samples do we need to produce a frame of output samples?
      mMonoResampler.setSpeed(speed);
      int need = mMonoResampler.required();

      if (mBuffering)
      {
        // silence
        memset(out, 0, sizeof(float) * FRAMELENGTH);
        if (have > 2 * need)
        {
          mBuffering = false;
          mFadeIn = true;
        }
      }
      else
      {
        if (have < need)
        {
          // silence
          memset(out, 0, sizeof(float) * FRAMELENGTH);
          mBuffering = true;
        }
        else
        {
          float *in = mFifo.front();
          int used = mMonoResampler.nextFrame(in, out);
          mFifo.pop(used);
        }
      }

      if (doReset)
      {
        float *fade = LookupTables::FrameOfLinearRamp.mValues.data();
        for (int i = 0; i < FRAMELENGTH; i++)
        {
          out[i] *= (1.0f - fade[i]);
        }
        mFifo.popAll();
        mBuffering = true;
      }
      else if (mFadeIn)
      {
        float *fade = LookupTables::FrameOfLinearRamp.mValues.data();
        for (int i = 0; i < FRAMELENGTH; i++)
        {
          out[i] *= fade[i];
        }
        mFadeIn = false;
      }
    }
    else
    {
      float *left = mLeftOutput.buffer();
      float *right = mRightOutput.buffer();

      if (mInputChannelCount == 2)
      {
        // How many input samples do we need to produce a frame of output samples?
        mStereoResampler.setSpeed(speed);
        int need = mStereoResampler.required();

        if (mBuffering)
        {
          // silence
          memset(left, 0, sizeof(float) * FRAMELENGTH);
          memset(right, 0, sizeof(float) * FRAMELENGTH);
          if (have > 2 * need)
          {
            mBuffering = false;
            mFadeIn = true;
          }
        }
        else
        {
          if (have < need)
          {
            // silence
            memset(left, 0, sizeof(float) * FRAMELENGTH);
            memset(right, 0, sizeof(float) * FRAMELENGTH);
            mBuffering = true;
          }
          else
          {
            float *in = mFifo.front();
            int used = mStereoResampler.nextFrame(in, left, right);
            mFifo.pop(used);
          }
        }
      }
      else
      {
        // How many input samples do we need to produce a frame of output samples?
        mMonoResampler.setSpeed(speed);
        int need = mMonoResampler.required();
        if (mBuffering)
        {
          // silence
          memset(left, 0, sizeof(float) * FRAMELENGTH);
          memset(right, 0, sizeof(float) * FRAMELENGTH);
          if (have > 2 * need)
          {
            mBuffering = false;
          }
        }
        else
        {
          if (have < need)
          {
            // silence
            memset(left, 0, sizeof(float) * FRAMELENGTH);
            memset(right, 0, sizeof(float) * FRAMELENGTH);
            mBuffering = true;
          }
          else
          {
            float *in = mFifo.front();
            int used = mMonoResampler.nextFrame(in, left);
            memcpy(right, left, sizeof(float) * FRAMELENGTH);
            mFifo.pop(used);
          }
        }
      }

      if (doReset)
      {
        float *fade = LookupTables::FrameOfLinearRamp.mValues.data();
        for (int i = 0; i < FRAMELENGTH; i++)
        {
          float w = (1.0f - fade[i]);
          left[i] *= w;
          right[i] *= w;
        }
        mFifo.popAll();
        mBuffering = true;
      }
      else if (mFadeIn)
      {
        float *fade = LookupTables::FrameOfLinearRamp.mValues.data();
        for (int i = 0; i < FRAMELENGTH; i++)
        {
          left[i] *= fade[i];
          right[i] *= fade[i];
        }
        mFadeIn = false;
      }
    }

    if (isLooping() || getPositionInSamples() < getDurationInSamples())
    {
      if (mRunning && mReadJob.finished() && mFifo.canPush(mSamplesPerChunk))
      {
        // schedule
        JobQueue *jobQ = UIThread::getRealtimeJobQueue();
        jobQ->push(&mReadJob);
      }
    }
  }

} /* namespace od */
