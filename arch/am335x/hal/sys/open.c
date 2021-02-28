#include "resource.h"
#include "ramdisk.h"
#include <unistd.h>
#include <errno.h>
#include "warning.h"
#include <fcntl.h>

//#define BUILDOPT_VERBOSE
//#define BUILDOPT_DEBUG_LEVEL 10
#include <hal/log.h>

#define TESTMASK(x, mask) (((x) & (mask)) == (mask))
#define TESTMASK2(x, mask, value) (((x) & (mask)) == (value))

static int ramdisk_open(char *path, int flags, int mode)
{
  size_t nbytes = 0;
  const char *buffer = NULL;
  int fd = createRamFileDescriptor();
  RamFile *fp = lookupRamFileDescriptor(fd);
  if (fp == NULL)
  {
    errno = EMFILE;
    return -1;
  }

  if (TESTMASK2(flags, O_ACCMODE, O_RDWR))
  {
    // bad
    errno = EROFS;
    goto on_fail;
  }
  else if (TESTMASK2(flags, O_ACCMODE, O_RDONLY))
  {
    // good
  }
  else if (TESTMASK2(flags, O_ACCMODE, O_WRONLY))
  {
    // bad
    errno = EROFS;
    goto on_fail;
  }

  if (TESTMASK(flags, O_CREAT) && TESTMASK(flags, O_EXCL))
  {
    // bad
    errno = EROFS;
    goto on_fail;
  }
  else if (TESTMASK(flags, O_CREAT))
  {
    // bad
    errno = EROFS;
    goto on_fail;
  }
  else
  {
    // good
  }

  if (*path == 'x' || *path == 'X')
  {
    path++;
  }
  else
  {
    errno = ENOENT;
    goto on_fail;
  }

  if (*path == ':')
  {
    path++;
  }
  else
  {
    errno = ENOENT;
    goto on_fail;
  }

  if (*path == '/')
  {
    path++;
  }

  buffer = ramdisk_lookup(path, &nbytes);
  if (buffer == NULL)
  {
    errno = ENOENT;
    goto on_fail;
  }
  fp->begin = buffer;
  fp->current = buffer;
  fp->nbytes = nbytes;

  if (TESTMASK(flags, O_TRUNC))
  {
    errno = EROFS;
    goto on_fail;
  }
  else if (TESTMASK(flags, O_APPEND))
  {
    errno = EROFS;
    goto on_fail;
  }

  return fd;

on_fail:
  destroyRamFileDescriptor(fd);
  return -1;
}

static int card_open(char *path, int flags, int mode)
{
  logDebug(1, "%s", path);
  BYTE f_mode = 0;
  FRESULT res;
  int fd = createCardFileDescriptor();
  file_t *fp = lookupCardFileDescriptor(fd);
  if (fp == NULL)
  {
    errno = EMFILE;
    return -1;
  }

#ifdef BUILDOPT_VERBOSE
  logDebug(1, "flags");

  if (TESTMASK2(flags, O_ACCMODE, O_RDONLY))
  {
    logDebug(1, "O_RDONLY");
  }

  if (TESTMASK2(flags, O_ACCMODE, O_WRONLY))
  {
    logDebug(1, "O_WRONLY");
  }

  if (TESTMASK2(flags, O_ACCMODE, O_RDWR))
  {
    logDebug(1, "O_RDWR");
  }

  if (TESTMASK(flags, O_CREAT))
  {
    logDebug(1, "O_CREAT");
  }

  if (TESTMASK(flags, O_TRUNC))
  {
    logDebug(1, "O_TRUNC");
  }

  if (TESTMASK(flags, O_APPEND))
  {
    logDebug(1, "O_APPEND");
  }
#endif

  if (TESTMASK2(flags, O_ACCMODE, O_RDONLY))
  {
    // r or rb
    f_mode |= FA_READ;
    f_mode |= FA_OPEN_EXISTING;
  }
  else if (TESTMASK(flags, O_RDWR) && !TESTMASK(flags, O_CREAT))
  {
    // r or rb
    f_mode |= FA_READ | FA_WRITE;
    f_mode |= FA_OPEN_EXISTING;
  }
  else if (TESTMASK(flags, O_CREAT | O_TRUNC))
  {
    // w or wb
    f_mode |= FA_WRITE | FA_READ;
    f_mode |= FA_CREATE_ALWAYS;
  }
  else if (TESTMASK(flags, O_CREAT | O_APPEND))
  {
    // a or ab
    f_mode |= FA_WRITE | FA_READ;
    f_mode |= FA_OPEN_ALWAYS;
  }

  res = f_open(&fp->fil, path, f_mode);
  if (res != FR_OK)
  {
    switch (res)
    {
    case FR_EXIST:
      errno = EEXIST;
      break;
    case FR_DENIED:
    case FR_WRITE_PROTECTED:
      errno = EACCES;
      break;
    case FR_NO_PATH:
    case FR_NO_FILE:
      errno = ENOENT;
      break;
    default:
      errno = ENFILE;
      break;
    }
    goto on_fail;
  }

  if (TESTMASK(flags, O_APPEND))
  {
    f_lseek(&fp->fil, f_size(&fp->fil));
#if FILE_READ_CACHE_ENABLED
    fp->pos = f_tell(&fp->fil);
#endif
  }

  return fd;

on_fail:
  destroyCardFileDescriptor(fd);
  return -1;
}

int _open(char *path, int flags, int mode)
{
  if (path && (path[0] == 'x' || path[0] == 'X') && (path[1] == ':'))
  {
    return ramdisk_open(path, flags, mode);
  }
  else
  {
    return card_open(path, flags, mode);
  }
}
