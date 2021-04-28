#include <od/audio/WavFileReader.h>
//#define BUILDOPT_VERBOSE
//#define BUILDOPT_DEBUG_LEVEL 10
#include <hal/log.h>
#include <hal/ops.h>
#include <string.h>
#include <stdio.h>

namespace od
{

  // WAVEFORMATEXTENSIBLE support

  const static GUID KSDATAFORMAT_SUBTYPE_PCM = {0x00000001, 0x0000, 0x0010, {0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71}};
  const static GUID KSDATAFORMAT_SUBTYPE_IEEE_FLOAT = {0x00000003, 0x0000, 0x0010, {0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71}};

  static std::string guidToString(const GUID &guid)
  {
    char tmp[256];
    snprintf(tmp, sizeof(tmp),
             "{%08X-%04hX-%04hX-%02hhX%02hhX-%02hhX%02hhX%02hhX%02hhX%02hhX%02hhX}",
             (unsigned int)guid.Data1, guid.Data2, guid.Data3, guid.Data4[0], guid.Data4[1],
             guid.Data4[2], guid.Data4[3], guid.Data4[4], guid.Data4[5],
             guid.Data4[6], guid.Data4[7]);
    return std::string(tmp); 
  }

  static inline int isEqualGUID(const GUID &rguid1, const GUID &rguid2)
  {
    return !memcmp(&rguid1, &rguid2, sizeof(GUID));
    //return guidToString(rguid1) == guidToString(rguid2);
  }

  /////////////////////

  WavFileReader::WavFileReader()
  {
  }

  WavFileReader::~WavFileReader()
  {
  }

  bool WavFileReader::open(const std::string &filename)
  {
    if (!SoundFileReader::open(filename))
    {
      logError("WavFileReader::open(%s): failed to open.",
               filename.c_str());
      return false;
    }

    if (!readHeader())
    {
      SoundFileReader::close();
      logError("WavFileReader::open(%s): failed to read header.",
               filename.c_str());
      return false;
    }

    if (getChannelCount() > 2)
    {
      SoundFileReader::close();
      logError("WavFileReader::open(%s): too many channels.",
               filename.c_str());
      return false;
    }

    return true;
  }

  bool WavFileReader::readHeader()
  {
    uint32_t br, next;
    RiffChunk header;
    FileTypeChunk riff;
    bool fmt_found = false;
    bool data_found = false;
    bool cue_found = false;

    // clear previous data
    memset(&mFormat, 0, sizeof(mFormat));
    mSampleCount = 0;
    mChannelCount = 0;
    mBitDepth = 0;
    mSampleRate = 0.0f;
    mTotalSeconds = 0.0f;
    mCurrentSamplePosition = 0;

    try
    {
      br = readBytes(&riff, sizeof(FileTypeChunk));
    }
    catch (const std::exception &e)
    {
      logError(
          "WavFileReader::readHeader: failed to read file type chunk (1).");
      return false;
    }

    if (br != sizeof(FileTypeChunk))
    {
      logError(
          "WavFileReader::readHeader: failed to read file type chunk (2).");
      return false;
    }

    if (riff.ckID[0] != 'R' || riff.ckID[1] != 'I' || riff.ckID[2] != 'F' || riff.ckID[3] != 'F')
    {
      logError(
          "WavFileReader::readHeader: first chunk was not RIFF: [%c%c%c%c]",
          riff.ckID[0], riff.ckID[1], riff.ckID[2], riff.ckID[3]);
      return false;
    }

    // iterate through all the subchunks
    next = sizeof(FileTypeChunk);
    while (!(fmt_found && data_found && cue_found))
    {

      if (seekBytes(next) != next)
      {
        logError("WavFileReader::readHeader: failed to seek to %d.",
                 next);
        return false;
      }
      br = readBytes(&header, sizeof(RiffChunk));
      if (br != sizeof(RiffChunk))
      {
        logError(
            "WavFileReader::readHeader: failed to read RiffChunk at %d.",
            next);
        return false;
      }

      logDebug(1, "WavFileReader:readHeader:chunk [%c%c%c%c] %d bytes",
               header.ckID[0], header.ckID[1], header.ckID[2], header.ckID[3],
               header.cksize);

      if ((header.ckID[0] == 'f' && header.ckID[1] == 'm' && header.ckID[2] == 't' && header.ckID[3] == ' ') || (header.ckID[0] == 'F' && header.ckID[1] == 'M' && header.ckID[2] == 'T' && header.ckID[3] == ' '))
      {
        uint32_t len = MIN(header.cksize, sizeof(WavFormatExData));
        br = readBytes(&mFormat, len);
        if (br != len)
          return false;
        if (mFormat.wFormatTag != WAV_FORMAT_PCM && mFormat.wFormatTag != WAV_FORMAT_FLOAT && mFormat.wFormatTag != WAV_FORMAT_EXTENSIBLE)
        {
          printFormat();
          logError(
              "WavFileReader::readHeader: unsupported format tag (%d).",
              mFormat.wFormatTag);
          return false;
        }
        if (mFormat.wFormatTag == WAV_FORMAT_EXTENSIBLE)
        {
          // format is in the sub-format field
          if (isEqualGUID(mFormat.SubFormat, KSDATAFORMAT_SUBTYPE_PCM))
          {
            mFormat.wFormatTag = WAV_FORMAT_PCM;
          }
          else if (isEqualGUID(mFormat.SubFormat,
                               KSDATAFORMAT_SUBTYPE_IEEE_FLOAT))
          {
            mFormat.wFormatTag = WAV_FORMAT_FLOAT;
          }
          else
          {
            printFormat();
            logError(
                "WavFileReader::readHeader: unsupported subtype format %s.",
                guidToString(mFormat.SubFormat).c_str());
            return false;
          }
        }
        mChannelCount = mFormat.nChannels;
        mSampleRate = (float)mFormat.nSamplesPerSec;
        mBytesPerChannel = mFormat.nBlockAlign / mFormat.nChannels;
        mBitDepth = mFormat.wBitsPerSample;
        if (mFormat.wFormatTag == WAV_FORMAT_FLOAT)
        {
          mDataIsFloat = true;
          if (mBytesPerChannel != 4)
          {
            printFormat();
            logError(
                "WavFileReader::readHeader: %d bytes per channel does not agree with float format.",
                mBytesPerChannel);
            return false;
          }
        }
        else
        {
          mDataIsFloat = false;
          if ( // 8-bit
              !(mFormat.wBitsPerSample == 8 && mBytesPerChannel == 1)
              // 16-bit
              && !(mFormat.wBitsPerSample == 16 && mBytesPerChannel == 2)
              // 24-bit
              && !(mFormat.wBitsPerSample == 24 && mBytesPerChannel == 3)
              // 32-bit
              && !(mFormat.wBitsPerSample == 32 && mBytesPerChannel == 4))
          {
            printFormat();
            logError(
                "WavFileReader::readHeader: unsupported bit depth (%d bits) and block size (%d bytes).",
                mFormat.wBitsPerSample, mBytesPerChannel);
            return false;
          }
        }
        fmt_found = true;
#ifdef BUILDOPT_VERBOSE
        printFormat();
#endif
      }
      else if ((header.ckID[0] == 'd' && header.ckID[1] == 'a' && header.ckID[2] == 't' && header.ckID[3] == 'a') || (header.ckID[0] == 'D' && header.ckID[1] == 'A' && header.ckID[2] == 'T' && header.ckID[3] == 'A'))
      {
        mDataPosition = sizeof(RiffChunk) + next;
        mSampleCount = header.cksize / mChannelCount / mBytesPerChannel;
        data_found = mSampleCount > 0;
      }
      else if ((header.ckID[0] == 'c' && header.ckID[1] == 'u' && header.ckID[2] == 'e' && header.ckID[3] == ' ') || (header.ckID[0] == 'C' && header.ckID[1] == 'U' && header.ckID[2] == 'E' && header.ckID[3] == ' '))
      {
        uint32_t extraHeaderSize = sizeof(CueChunkData);
        uint32_t len = MIN(header.cksize, extraHeaderSize);

        CueChunkData cueHeader;
        br = readBytes(&cueHeader, len);
        if (br != len) return false;

        mCueDataSize = header.cksize;
        mCuePointsPosition = sizeof(RiffChunk) + extraHeaderSize + next;
        mCuePointsCount = cueHeader.numCuePoints;

        cue_found = true;
      }

      next += sizeof(RiffChunk) + header.cksize;
      // pad byte, if the chunk's length is not even
      if ((header.cksize & 0x1) == 0x1)
      {
        next++;
      }
      if (next >= getSizeInBytes())
        break;
    }

    if (!fmt_found || !data_found)
    {
      if (!fmt_found)
        logError(
            "WavFileReader::readHeader: could not find 'fmt' chunk.");
      if (!data_found)
        logError(
            "WavFileReader::readHeader: could not find 'data' chunk.");
      return false;
    }

    if (seekBytes(mDataPosition) != mDataPosition)
    {
      logError(
          "WavFileReader::readHeader: could not seek to data position (%d).",
          mDataPosition);
      return false;
    }

    mTotalSeconds = (double)mSampleCount / (double)mSampleRate;
    return true;
  }

  uint32_t WavFileReader::readSamples(float *buffer, uint32_t len)
  {
    uint32_t br, sr = 0;

    if (mDataIsFloat)
    {
      br = readBytes((char *)buffer, mChannelCount * len * sizeof(float));
      sr = br / sizeof(float);
    }
    else if (mFormat.wBitsPerSample == 8 && mBytesPerChannel == 1)
    {
      // 8 bit (or lower) WAV files are always unsigned. 9 bit or higher are always signed.
      uint32_t i;
      mUInt8Buffer.resize(len * mChannelCount);
      br = readBytes(mUInt8Buffer.data(),
                     mChannelCount * len * sizeof(int8_t));
      sr = br / sizeof(int8_t);
      for (i = 0; i < sr; i++)
      {
        buffer[i] = ((float)mUInt8Buffer[i]) * (1.0f / 255) - 0.5f;
      }
    }
    else if (mFormat.wBitsPerSample == 16 && mBytesPerChannel == 2)
    {
      uint32_t i;
      mInt16Buffer.resize(len * mChannelCount);
      br = readBytes(mInt16Buffer.data(),
                     mChannelCount * len * sizeof(int16_t));
      sr = br / sizeof(int16_t);
      for (i = 0; i < sr; i++)
      {
        buffer[i] = ((float)mInt16Buffer[i]) * (1.0f / (1 << 15));
      }
    }
    else if (mFormat.wBitsPerSample == 24 && mBytesPerChannel == 3)
    {
      uint32_t i, j, n = len * mChannelCount * 3;
      mUInt8Buffer.resize(n);
      int32_t value;
      br = readBytes(mUInt8Buffer.data(), n);
      sr = br / 3;
      for (i = 0, j = 0; i < br; i += 3, j++)
      {
        value = (mUInt8Buffer[i] << 8) | (mUInt8Buffer[i + 1] << 16) | (mUInt8Buffer[i + 2] << 24);
        buffer[j] = ((float)(value >> 8)) * (1.0f / (1 << 23));
      }
    }
    else if (mFormat.wBitsPerSample == 32 && mBytesPerChannel == 4)
    {
      uint32_t i;
      mInt32Buffer.resize(len * mChannelCount);
      br = readBytes(mInt32Buffer.data(),
                     mChannelCount * len * sizeof(int32_t));
      sr = br / sizeof(int32_t);
      for (i = 0; i < sr; i++)
      {
        buffer[i] = ((float)mInt32Buffer[i]) * (1.0f / (1 << 31));
      }
    }
    else
    {
      return 0;
    }

    sr /= mChannelCount;
    mCurrentSamplePosition += sr;
    return sr;
  }

  uint32_t WavFileReader::seekSamples(uint32_t offset)
  {
    int target = mDataPosition + offset * mFormat.nBlockAlign;
    int actual = seekBytes(target);
    if (target == actual)
    {
      mCurrentSamplePosition = offset;
    }
    else
    {
      mCurrentSamplePosition = (actual - mDataPosition) / mFormat.nBlockAlign;
    }
    return mCurrentSamplePosition;
  }

  uint32_t WavFileReader::tellSamples()
  {
    return mCurrentSamplePosition;
  }

  bool WavFileReader::readCuePositions(std::vector<uint32_t> &positions)
  {
    uint32_t br, pr = 0;

    if (seekBytes(mCuePointsPosition) != mCuePointsPosition)
    {
      logError(
          "WavFileReader::readCuePositions: could not seek to cue position (%d).",
          mCuePointsPosition);
      return false;
    }

    for (int i = 0; i < mCuePointsCount; i++) {
      CueFormatData data;
      uint32_t size = sizeof(CueFormatData);
      br = readBytes(&data, size);
      if (br != size) return false;

      positions.push_back(data.position);
    }

    return true;
  }

  void WavFileReader::describe()
  {
    logInfo("-- %s -- ", mFilename.c_str());
    logInfo("  Sample Rate: %d Hz", mFormat.nSamplesPerSec);
    logInfo("  Channels: %d", mChannelCount);
    logInfo("  Bit Depth: %d bits", mFormat.wBitsPerSample);
    logInfo("  Bytes/Channel: %d bytes", mBytesPerChannel);
    logInfo("  Data Position: %d", mDataPosition);
    logInfo("  Number of Samples: %d samples", mSampleCount);
    logInfo("  Cue Position: %d", mCuePointsPosition);
    logInfo("  Number of Cue Points: %d", mCuePointsCount);

    if (mDataIsFloat)
      logInfo("  Format: float");
    else
      logInfo("  Format: signed int");
  }

  uint32_t WavFileReader::getCueCount()
  {
    return mCuePointsCount;
  }

  void WavFileReader::printFormat()
  {
    logInfo("wFormatTag: %d ", mFormat.wFormatTag);
    logInfo("nChannels: %d ", mFormat.nChannels);
    logInfo("nSamplesPerSec: %d ", mFormat.nSamplesPerSec);
    logInfo("nAvgBytesPerSec: %d ", mFormat.nAvgBytesPerSec);
    logInfo("nBlockAlign: %d ", mFormat.nBlockAlign);
    logInfo("wBitsPerSample: %d ", mFormat.wBitsPerSample);
    if (mFormat.wFormatTag == WAV_FORMAT_EXTENSIBLE)
    {
      logInfo("cbSize: %d ", mFormat.cbSize);
      logInfo("wValidBitsPerSample: %d",
              mFormat.wValidBitsPerSample);
      logInfo("dwChannelMask: %d ", mFormat.dwChannelMask);
      logInfo("SubFormat: %s",
              guidToString(mFormat.SubFormat).c_str());
    }
  }

} /* namespace od */
