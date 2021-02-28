#include "resource.h"
#include <unistd.h>
#include <errno.h>
#include "warning.h"
#include <hal/log.h>

int fsync(int fd)
{
  if (isCardFileDescriptor(fd))
  {
    FRESULT res;
    file_t *fp = lookupCardFileDescriptor(fd);

    if (fp == NULL)
    {
      errno = EBADF;
      return -1;
    }

    res = f_sync(&fp->fil);
    if (res != FR_OK)
    {
      logWarn("f_sync failed. (res = %d)", res);
      errno = EIO;
      return -1;
    }

    return 0;
  }
  else
  {
    /* Writing to a RamFile is not implemented. */
    errno = EBADF;
    return -1;
  }
}