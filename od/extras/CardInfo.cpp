#include <od/extras/CardInfo.h>
#include <od/extras/Utils.h>
#include <od/extras/SystemBuffer.h>
#include <od/extras/FileWriter.h>
#include <od/extras/FileReader.h>
#include <hal/card.h>
#define BUILDOPT_VERBOSE
#define BUILDOPT_DEBUG_LEVEL 10
#include <hal/log.h>
#include <hal/timing.h>
#include <hal/fileops.h>

namespace od
{

  CardInfo::CardInfo(const std::string &path) : mPath(path)
  {
  }

  CardInfo::~CardInfo()
  {
  }

  bool CardInfo::measureSpeed(int nbytes, int ntrials, int chunkSize)
  {
    FileWriter writer;
    FileReader reader;
    tick_t start, end;
    SystemBuffer chunk;

    mWriteSpeed = 0.0f;
    mReadSpeed = 0.0f;

    std::string filename = suggestTempFilename(mPath);
    if (filename == "")
    {
      logError("CardInfo::measureSpeed - could not get temp file ");
      return false;
    }

    if (!chunk.allocate(chunkSize))
    {
      logError("CardInfo::measureSpeed - failed to allocate buffer ");
      return false;
    }

    for (int i = 0; i < chunkSize; i++)
    {
      chunk.mpData[i] = (char)i;
    }

    if (!writer.open(filename))
    {
      logError("CardInfo::measureSpeed - failed to create %s for writing.",
               filename.c_str());
      return false;
    }

    start = ticks();
    for (int i = 0; i < ntrials + 1; i++)
    {
      logDebug(1,"write trial %d", i);
      writer.seekBytes(0);
      if (i == 1)
      {
        // Ignore the first trial so that timing is not influenced by FAT-related block searches.
        start = ticks();
      }
      for (int j = 0; j < nbytes; j += chunkSize)
      {
        int bw = writer.writeBytes(chunk.mpData, chunkSize);
        if (bw != chunkSize)
        {
          logError("CardInfo::measureSpeed - failed to write %d bytes @ offset %d",
                   chunkSize, j);
          goto error;
        }
      }
    }
    end = ticks();
    writer.close();
    mWriteSpeed = (ntrials * nbytes) / ticks2secs(end - start);

    if (!reader.open(filename))
    {
      logError("CardInfo::measureSpeed - failed to open %s for reading.",
               filename.c_str());
      goto error;
    }

    start = ticks();
    for (int i = 0; i < ntrials; i++)
    {
      logDebug(1,"read trial %d", i);
      reader.seekBytes(0);
      for (int j = 0; j < nbytes; j += chunkSize)
      {
        int br = reader.readBytes(chunk.mpData, chunkSize);
        if (br != chunkSize)
        {
          logError("CardInfo::measureSpeed - failed to read %d bytes @ offset %d.",
                   chunkSize, j);
          goto error;
        }
      }
    }
    end = ticks();
    mReadSpeed = (ntrials * nbytes) / ticks2secs(end - start);
    reader.close();
    deleteFile(filename.c_str());

    logInfo("write speed: %f bytes/sec", mWriteSpeed);
    logInfo("read speed: %f bytes/sec", mReadSpeed);
    return true;

  error:
    deleteFile(filename.c_str());
    return false;
  }

  bool CardInfo::isHighCapacity()
  {
    return Card_isHighCapacity(1);
  }

  int CardInfo::getVersion()
  {
    return Card_getVersion(1);
  }

  int CardInfo::getBusWidth()
  {
    return Card_getBusWidth(1);
  }

  int CardInfo::getTransferSpeed()
  {
    return Card_getTransferSpeed(1);
  }

  bool CardInfo::supportsCMD23()
  {
    return Card_supportsCMD23(1);
  }

} /* namespace od */
