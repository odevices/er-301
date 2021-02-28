/*
 * Stub version of read.
 */

#include "resource.h"
#include <unistd.h>
#include <errno.h>
#include "warning.h"
#include <hal/ops.h>
//#define BUILDOPT_VERBOSE
//#define BUILDOPT_DEBUG_LEVEL 10
#include <hal/log.h>
#include <string.h>

#if FILE_READ_CACHE_ENABLED

static uint32_t fill_cache(file_t *fp, uint32_t offset, FRESULT *res)
{
  UINT br = 0;
  uint32_t len = MIN(FILE_READ_CACHE_LEN, f_size(&fp->fil) - offset);
  fp->cache.offset = offset;
  logDebug(1, "%d bytes @ %d", len, offset);
  if (offset != f_tell(&fp->fil))
  {
    *res = f_lseek(&fp->fil, offset);
    if (*res != FR_OK)
    {
      return 0;
    }
  }
  *res = f_read(&fp->fil, fp->cache.data, len, &br);
  fp->cache.len = br;
  return br;
}

static uint32_t direct_read(file_t *fp, char *buffer, uint32_t len, uint32_t offset, FRESULT *res)
{
  UINT br = 0;
  if (offset != f_tell(&fp->fil))
  {
    *res = f_lseek(&fp->fil, offset);
    if (*res != FR_OK)
    {
      return 0;
    }
  }

  *res = f_read(&fp->fil, buffer, len, &br);
  return br;
}

static uint32_t cached_read(file_t *fp, char *buffer, uint32_t len, uint32_t offset, FRESULT *res)
{
  logDebug(1, "read=%d, offset=%d, fp.cache{.offset=%d, .len=%d}, fp.pos=%d", len, offset, fp->cache.offset, fp->cache.len, fp->pos);

  // cached interval
  int c0 = fp->cache.offset;
  int c1 = fp->cache.offset + fp->cache.len;
  // read interval
  int r0 = offset;
  int r1 = MIN(offset + len, f_size(&fp->fil));
  // intersection
  int i0 = MAX(r0, c0);
  int i1 = MIN(r1, c1);
  // Adjust the actual read length for file size.
  len = r1 - r0;
  if (len == 0)
  {
    fp->pos = f_size(&fp->fil);
    return 0;
  }

  logDebug(1, "r=[%d,%d] c=[%d,%d] i=[%d,%d]", r0, r1, c0, c1, i0, i1);

  if (i0 >= i1 || r0 < i0)
  {
    // Read and cached regions are disjoint, or
    // first part of the read region is not cached.
    uint32_t br = fill_cache(fp, offset, res);
    len = MIN(br, len);
    if (len > 0)
    {
      memcpy(buffer, fp->cache.data, len);
    }
    fp->pos = offset + len;
    return len;
  }

  if (i0 == r0 && i1 == r1)
  {
    // Read area is completely cached.
    memcpy(buffer, fp->cache.data + r0 - c0, len);
    fp->pos = offset + len;
    return len;
  }

  if (i0 == c0 && i1 == c1)
  {
    int br = 0;
    // Read area completely contains the cached area.
    if (r0 < c0)
    {
      // Read the first (uncached) segment.
      logDebug(1, "Cache miss: %d bytes", c0 - r0);
      br += direct_read(fp, buffer, c0 - r0, offset, res);
      if (br < c0 - r0)
      {
        // Failed to read desired length, so stop here.
        fp->pos = offset + br;
        return br;
      }
    }

    if (i0 < i1)
    {
      // Copy the cached portion.
      memcpy(buffer + c0 - r0, fp->cache.data, fp->cache.len);
      br += fp->cache.len;
    }

    if (c1 < r1)
    {
      // Read the last (uncached) chunk.
      fill_cache(fp, c1, res);
      len = MIN(fp->cache.len, r1 - c0);
      if (len > 0)
      {
        memcpy(buffer + c1 - r0, fp->cache.data, len);
        br += len;
      }
    }

    fp->pos = offset + br;
    return br;
  }

  if (i1 < r1)
  {
    // First part of the read region is cached.
    memcpy(buffer, fp->cache.data + r0 - c0, c1 - r0);
    fill_cache(fp, c1, res);
    int remaining = MIN(fp->cache.len, r1 - c1);
    if (remaining > 0)
    {
      memcpy(buffer + c1 - r0, fp->cache.data, remaining);
    }
    fp->pos = offset + c1 - r0 + remaining;
    return c1 - r0 + remaining;
  }

  logError("Should not get here. r=[%d,%d] c=[%d,%d] i=[%d,%d]",
           r0, r1, c0, c1, i0, i1);
  return 0;
}

static int card_read(int fd, char *ptr, int len)
{
  UINT br = 0;
  FRESULT res = FR_OK;
  file_t *fp = lookupCardFileDescriptor(fd);

  if (fp == NULL)
  {
    errno = EBADF;
    return -1;
  }

  if (len > FILE_READ_CACHE_LEN / 2)
  {
    // Skip caching if the read size is more than half the maximum cache size.
    logDebug(1, "Cache miss: %d bytes", len);
    if (fp->pos != f_tell(&fp->fil))
    {
      res = f_lseek(&fp->fil, fp->pos);
      if (res != FR_OK)
      {
        errno = EIO;
        return -1;
      }
    }
    res = f_read(&fp->fil, ptr, len, &br);
  }
  else
  {
    br = cached_read(fp, ptr, len, fp->pos, &res);
  }

  if (res != FR_OK)
  {
    errno = EIO;
    return -1;
  }

  return br;
}

#else

static int card_read(int fd, char *ptr, int len)
{
  UINT br = 0;
  FRESULT res = FR_OK;
  file_t *fp = lookupCardFileDescriptor(fd);

  if (fp == NULL)
  {
    errno = EBADF;
    return -1;
  }

  res = f_read(&fp->fil, ptr, len, &br);
  if (res != FR_OK)
  {
    errno = EIO;
    return -1;
  }

  return br;
}

#endif

static int ramdisk_read(int fd, char *ptr, int len)
{
  int br = 0;
  RamFile *fp = lookupRamFileDescriptor(fd);

  if (fp == NULL || fp->current == NULL)
  {
    errno = EBADF;
    return -1;
  }

  if (ptr)
  {
    br = MIN(len, fp->nbytes - (fp->current - fp->begin));
    if (br > 0)
    {
      memcpy(ptr, fp->current, br);
      fp->current += br;
    }

    return br;
  }
  else
  {
    return 0;
  }
}

int _read(int fd, char *ptr, int len)
{
  if (isCardFileDescriptor(fd))
  {
    return card_read(fd, ptr, len);
  }
  else if (isRamFileDescriptor(fd))
  {
    return ramdisk_read(fd, ptr, len);
  }
  else
  {
    errno = EBADF;
    return -1;
  }
}
