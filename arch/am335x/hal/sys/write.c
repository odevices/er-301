#include "resource.h"
#include <unistd.h>
#include <errno.h>
#include "warning.h"
//#define BUILDOPT_VERBOSE
//#define BUILDOPT_DEBUG_LEVEL 10
#include <hal/log.h>

int _write(int fd, char *ptr, int len)
{
  if (isCardFileDescriptor(fd))
  {
    UINT bw;
    FRESULT res;
    file_t *fp = lookupCardFileDescriptor(fd);

    if (fp == NULL)
    {
      errno = EBADF;
      return -1;
    }

#if FILE_READ_CACHE_ENABLED
    if (fp->pos != f_tell(&fp->fil))
    {
      res = f_lseek(&fp->fil, fp->pos);
      if (res != FR_OK)
      {
        errno = EIO;
        return -1;
      }
    }
#endif

    logDebug(1, "%d bytes", len);
    res = f_write(&fp->fil, ptr, len, &bw);
    if (res != FR_OK)
    {
      errno = EIO;
      return -1;
    }

#if FILE_READ_CACHE_ENABLED
    fp->pos = f_tell(&fp->fil);
#endif

    return bw;
  }
  else
  {
    /* Writing to a RamFile is not implemented. */
    errno = EBADF;
    return -1;
  }
}
