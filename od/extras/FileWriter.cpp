#include <od/extras/FileWriter.h>
#include <string.h>
#ifdef FILEWRITER_USE_SYS
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#endif

namespace od
{

  FileWriter::FileWriter() : mIsOpen(false)
  {
  }

  FileWriter::~FileWriter()
  {
    close();
  }

  bool FileWriter::open(const std::string &filename)
  {
    if (mIsOpen)
      close();
    mFilename = filename;
#ifdef FILEWRITER_USE_SYS
    mFileDescriptor = ::open(mFilename.c_str(),
                             O_WRONLY | O_CREAT | O_TRUNC,
                             S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH);
    mIsOpen = mFileDescriptor != 0;
#else
    mIsOpen = f_open(&mFileDescriptor, mFilename.c_str(),
                     FA_READ | FA_WRITE | FA_CREATE_ALWAYS) == FR_OK;
#endif
    return mIsOpen;
  }

  bool FileWriter::close()
  {
    if (mIsOpen)
    {
      mIsOpen = false;
#ifdef FILEWRITER_USE_SYS
      ::close(mFileDescriptor);
#else
      f_close(&mFileDescriptor);
#endif
    }

    return true;
  }

  void FileWriter::flush()
  {
    if (mIsOpen)
    {
#ifdef FILEWRITER_USE_SYS
      ::fsync(mFileDescriptor);
#else
      f_sync(&mFileDescriptor);
#endif
    }
  }

  bool FileWriter::writeHeader(const char *description, uint32_t major,
                               uint32_t minor)
  {
    uint32_t magic = 0xABCDDCBA;
    uint32_t n = strlen(description) + 1;
    bool result = true;
    result = result && writeBytes(&magic, sizeof(uint32_t));
    result = result && writeBytes(&n, sizeof(uint32_t));
    result = result && writeBytes(description, n);
    result = result && writeBytes(&major, sizeof(uint32_t));
    result = result && writeBytes(&major, sizeof(uint32_t));
    return result;
  }

  uint32_t FileWriter::writeBytes(const void *buffer, uint32_t len)
  {
#ifdef FILEWRITER_USE_SYS
    uint32_t bw = ::write(mFileDescriptor, buffer, len);
#else
    UINT bw;
    FRESULT result = f_write(&mFileDescriptor, buffer, len, &bw);
    if (result != FR_OK)
    {
      return 0;
    }
#endif
    return bw;
  }

  uint32_t FileWriter::seekBytes(uint32_t offset)
  {
#ifdef FILEWRITER_USE_SYS
    return ::lseek(mFileDescriptor, offset, SEEK_SET);
#else
    if (f_lseek(&mFileDescriptor, offset) != FR_OK)
    {
      return 0;
    }
    return f_tell(&mFileDescriptor);
#endif
  }

  uint32_t FileWriter::tellBytes()
  {
#ifdef FILEWRITER_USE_SYS
    return ::lseek(mFileDescriptor, 0, SEEK_CUR);
#else
    return f_tell(&mFileDescriptor);
#endif
  }

} /* namespace od */
