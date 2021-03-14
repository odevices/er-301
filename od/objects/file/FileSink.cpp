#include <od/objects/file/FileSink.h>
#include <od/objects/file/FileSinkThread.h>
#include <od/config.h>
#include <hal/fileops.h>
#include <hal/log.h>

namespace od
{

  FileSink::FileSink(int channelCount) : mChannelCount(channelCount)
  {
  }

  FileSink::~FileSink()
  {
    stop();
    close();
  }

  void FileSink::setFilename(const std::string &filename)
  {
    mFilename = filename;
  }

  void FileSink::setEncoding(WavFileEncoding encoding)
  {
    mEncoding = encoding;
  }

  void FileSink::start()
  {
    mStarted = true;
    mAbort = false;
  }

  void FileSink::stop()
  {
    mStarted = false;
  }

  void FileSink::abort()
  {
    mStarted = false;
    mAbort = true;
  }

  bool FileSink::open()
  {
    close();

    if (pathExists(mFilename.c_str()))
    {
      logInfo("Deleting %s", mFilename.c_str());
      deleteFile(mFilename.c_str());
    }

    mWriter.init(globalConfig.sampleRate, mChannelCount, mEncoding);
    if (mWriter.open(mFilename))
    {
      logInfo("Opened %s", mFilename.c_str());
      mError = false;
      return true;
    }
    else
    {
      mError = true;
      return false;
    }
  }

  void FileSink::close()
  {
    if (mWriter.mIsOpen)
    {
      mWriter.close();
      logInfo("Closed %s", mFilename.c_str());
    }
  }

  float *FileSink::getWriteBuffer()
  {
    if (mWriteBuffer && mWritePosition < mpThread->getBufferSize())
    {
      return mWriteBuffer + mWritePosition;
    }
    else
    {
      mWriteBuffer = mpThread->requestBuffer();
      mWritePosition = 0;
      return mWriteBuffer;
    }
  }

  void FileSink::putWriteBuffer()
  {
    mWritePosition += mChannelCount * globalConfig.frameLength;
    if (mWritePosition >= mpThread->getBufferSize())
    {
      mFilledBuffers.push(mWriteBuffer);
      mpThread->notifyBufferReady();
    }
  }

  int FileSink::getPendingBufferCount()
  {
    return (int)mFilledBuffers.count();
  }

  void FileSink::writeBuffersToFile()
  {
    int n = mpThread->getBufferSize() / mChannelCount;
    float *buffer = 0;
    while (!mAbort && mWriter.mIsOpen && mFilledBuffers.pull(&buffer))
    {
      int written = (int)mWriter.writeSamples(buffer, n);
      mpThread->releaseBuffer(buffer);
      if (written < n)
      {
        stop();
        close();
        mError = true;
        break;
      }
    }
    if (mAbort)
    {
      while (mFilledBuffers.pull(&buffer))
      {
        mpThread->releaseBuffer(buffer);
      }
    }
  }

  void FileSink::setThread(FileSinkThread *thread)
  {
    if (mpThread)
    {
      // release any buffers that have not been written yet
      float *buffer = 0;
      while (mFilledBuffers.pull(&buffer))
      {
        mpThread->releaseBuffer(buffer);
      }
    }
    mpThread = thread;
  }

} /* namespace od */
