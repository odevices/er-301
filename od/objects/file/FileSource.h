#pragma once

#include <od/objects/Object.h>
#include <od/audio/MonoResampler.h>
#include <od/audio/StereoResampler.h>
#include <od/audio/WavFileReader.h>
#include <od/ui/JobQueue.h>
#include <od/audio/SampleFifo.h>

namespace od
{

  class FileSource : public Object
  {
  public:
    FileSource(int channelCount);
    virtual ~FileSource();

#ifndef SWIGLUA
    virtual void process();
    Outlet mOutput{"Out"};
    Outlet mLeftOutput{"Left Out"};
    Outlet mRightOutput{"Right Out"};
    Parameter mSpeed{"Speed", 1.0f};
    Parameter mPosition{"Position", 0.0f};
    Parameter mReset{"Reset"};
    Option mLoop{"How Often", HOWOFTEN_LOOP};
#endif

    bool open(const std::string &filename);
    void close();

    bool error()
    {
      return mError;
    }

    bool buffering()
    {
      return mBuffering;
    }

    bool playing()
    {
      return mRunning && !mError;
    }

    void pause()
    {
      mPaused = true;
    }

    void unpause()
    {
      mPaused = false;
    }

    bool isPaused()
    {
      return mPaused;
    }

    bool isEOF()
    {
      return mEOF;
    }

    void setPositionInSamples(uint32_t position);
    uint32_t getPositionInSamples()
    {
      if (mReader.mIsOpen)
      {
        int pos = (int)mReader.tellSamples();
        pos -= mFifo.size();
        if (pos < 0)
        {
          pos += mReader.getSampleCount();
        }
        return pos;
      }
      else
      {
        return 0;
      }
    }
    float getPositionInSeconds()
    {
      return getPositionInSamples() * mSamplePeriod;
    }

    float getSampleRate()
    {
      return mReader.getSampleRate();
    }

    float getDurationInSeconds()
    {
      if (mReader.mIsOpen)
      {
        return mReader.getSampleCount() * mSamplePeriod;
      }
      else
      {
        return 0;
      }
    }

    uint32_t getDurationInSamples()
    {
      if (mReader.mIsOpen)
      {
        return mReader.getSampleCount();
      }
      else
      {
        return 0;
      }
    }

    const std::string &filename()
    {
      return mFilename;
    }

    float getBufferDepthInSeconds();
    float getMaxBufferDepthInSeconds();

    void setLooping(bool value)
    {
      if (value)
      {
        mLoop.set(HOWOFTEN_LOOP);
      }
      else
      {
        mLoop.set(HOWOFTEN_ONCE);
      }
    }

    bool isLooping()
    {
      return mLoop.value() == HOWOFTEN_LOOP;
    }

  protected:
    int mOutputChannelCount;
    int mInputChannelCount = 1;
    bool mRunning = false;
    bool mError = false;
    bool mBuffering = true;
    bool mFadeIn = false;
    bool mPaused = false;
    bool mEOF = true;

    SampleFifo mFifo;
    MonoResampler mMonoResampler;
    StereoResampler mStereoResampler;
    WavFileReader mReader;
    std::string mFilename;
    float mSamplePeriod = 0.0f;

    class ReadJob : public Job
    {
    public:
      ReadJob(FileSource *source) : mpSource(source)
      {
      }
      FileSource *mpSource;
      virtual void work()
      {
        mpSource->readWork();
      }
    };

    ReadJob mReadJob;
    uint32_t mRequestedPosition = 0;
    bool mHavePendingSeek = false;

    void start();
    void stop();
    void readWork();

    bool readSamples(float *buffer, int n);
    int mSamplesPerChunk;
    Sample mChunkBuffer;
  };

} /* namespace od */
