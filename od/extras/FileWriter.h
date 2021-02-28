#pragma once

#include <od/extras/ReferenceCounted.h>
#include <string>

#define FILEWRITER_USE_SYS
#ifndef FILEWRITER_USE_SYS
#include <hal/fatfs/ff.h>
#endif

namespace od
{

  class FileWriter : public ReferenceCounted
  {
  public:
    FileWriter();
    virtual ~FileWriter();

    virtual bool open(const std::string &filename);
    virtual bool close();

    void flush();
    bool writeHeader(const char *description, uint32_t major, uint32_t minor);
    uint32_t writeBytes(const void *buffer, uint32_t len);
    uint32_t seekBytes(uint32_t offset);
    uint32_t tellBytes();

    std::string mFilename;
    bool mIsOpen;

  private:
#ifdef FILEWRITER_USE_SYS
    int mFileDescriptor;
#else
    FIL mFileDescriptor;
#endif
  };

} /* namespace od */
