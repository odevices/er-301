/*
 * Stub version of lseek.
 */

#include "resource.h"
#include <unistd.h>
#include <errno.h>
#include "warning.h"
#include <stdio.h>

#if FILE_READ_CACHE_ENABLED

static int card_lseek(int fd, int offset, int whence)
{
  FRESULT res;
  DWORD offset_from_beginning;
  file_t *fp = lookupCardFileDescriptor(fd);

  if (fp == NULL)
  {
    errno = EBADF;
    return -1;
  }

  switch (whence)
  {
  case SEEK_SET:
    offset_from_beginning = offset;
    break;
  case SEEK_CUR:
    offset_from_beginning = fp->pos + offset;
    break;
  case SEEK_END:
    offset_from_beginning = f_size(&fp->fil) + offset;
    break;
  default:
    errno = EINVAL;
    return -1;
  }

  if (offset_from_beginning == fp->pos)
  {
    // Seek not required, just return the current position.
    return fp->pos;
  }

  res = f_lseek(&fp->fil, offset_from_beginning);
  if (res != FR_OK)
  {
    errno = EINVAL;
    return -1;
  }

  fp->pos = f_tell(&fp->fil);
  return fp->pos;
}

#else

static int card_lseek(int fd, int offset, int whence)
{
  FRESULT res;
  DWORD offset_from_beginning;
  file_t *fp = lookupCardFileDescriptor(fd);

  if (fp == NULL)
  {
    errno = EBADF;
    return -1;
  }

  switch (whence)
  {
  case SEEK_SET:
    offset_from_beginning = offset;
    break;
  case SEEK_CUR:
    offset_from_beginning = f_tell(&fp->fil) + offset;
    break;
  case SEEK_END:
    offset_from_beginning = f_size(&fp->fil) + offset;
    break;
  default:
    errno = EINVAL;
    return -1;
  }

  res = f_lseek(&fp->fil, offset_from_beginning);
  if (res != FR_OK)
  {
    errno = EINVAL;
    return -1;
  }

  return f_tell(&fp->fil);
}

#endif

static int ramdisk_lseek(int fd, int offset, int whence)
{
  DWORD offset_from_beginning;
  RamFile *fp = lookupRamFileDescriptor(fd);

  if (fp == NULL || fp->current == NULL)
  {
    errno = EBADF;
    return -1;
  }

  switch (whence)
  {
  case SEEK_SET:
    if (offset < 0)
    {
      offset_from_beginning = 0;
    }
    else if (offset > fp->nbytes)
    {
      offset_from_beginning = fp->nbytes;
    }
    else
    {
      offset_from_beginning = offset;
    }
    break;
  case SEEK_CUR:
    if (offset < -(fp->current - fp->begin))
    {
      offset_from_beginning = 0;
    }
    else if ((fp->current - fp->begin) + offset > fp->nbytes)
    {
      offset_from_beginning = fp->nbytes;
    }
    else
    {
      offset_from_beginning = (fp->current - fp->begin) + offset;
    }
    break;
  case SEEK_END:
    if (offset < -fp->nbytes)
    {
      offset_from_beginning = 0;
    }
    if (offset > 0)
    {
      offset_from_beginning = fp->nbytes;
    }
    else
    {
      offset_from_beginning = fp->nbytes + offset;
    }
    break;
  default:
    errno = EINVAL;
    return -1;
  }

  fp->current = fp->begin + offset_from_beginning;
  return (fp->current - fp->begin);
}

int _lseek(int fd, int offset, int whence)
{
  if (isCardFileDescriptor(fd))
  {
    return card_lseek(fd, offset, whence);
  }
  else if (isRamFileDescriptor(fd))
  {
    return ramdisk_lseek(fd, offset, whence);
  }
  else
  {
    errno = EBADF;
    return -1;
  }
}
