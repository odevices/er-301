#include <hal/log.h>
#include <hal/heap.h>
#include "resource.h"
#include <_ansi.h>
#include <_syslist.h>
#include <errno.h>
#include "warning.h"
#include <fcntl.h>
#include <string.h>

typedef struct
{
  void *data;
  int type;
} entry_t;

#define MAX_NUM_FILDES 128
#define FILDES_OFFSET 4
#define FILDES_RAM 1
#define FILDES_CARD 2
static entry_t entries[MAX_NUM_FILDES] = {{0}};

static int indexFromCardFileDescriptor(int fd)
{
  int i = fd - FILDES_OFFSET;
  if (i < 0 || i >= MAX_NUM_FILDES)
  {
    return -1;
  }
  if (entries[i].data == NULL || entries[i].type != FILDES_CARD)
  {
    return -1;
  }
  return i;
}

int createCardFileDescriptor(void)
{
  int i;

  for (i = 0; i < MAX_NUM_FILDES; i++)
  {
    if (entries[i].data == NULL)
    {
      file_t *fp = (file_t *)Heap_calloc(1, sizeof(file_t));
      logAssert(fp);
#if FILE_READ_CACHE_ENABLED
      fp->cache.data = (char *)Heap_malloc(FILE_READ_CACHE_LEN);
#endif
      entries[i].data = (void *)fp;
      entries[i].type = FILDES_CARD;
      return i + FILDES_OFFSET;
    }
  }

  return -1;
}

void destroyCardFileDescriptor(int fd)
{
  int i = indexFromCardFileDescriptor(fd);
  if (i < 0)
    return;
  file_t *fp = (file_t *)(entries[i].data);
#if FILE_READ_CACHE_ENABLED
  Heap_free(fp->cache.data);
#endif
  Heap_free(fp);
  entries[i].data = NULL;
}

file_t *lookupCardFileDescriptor(int fd)
{
  int i = indexFromCardFileDescriptor(fd);
  if (i < 0)
    return NULL;
  return (file_t *)(entries[i].data);
}

bool isCardFileDescriptor(int fd)
{
  return indexFromCardFileDescriptor(fd) >= 0;
}

static int indexFromRamFileDescriptor(int fd)
{
  int i = fd - FILDES_OFFSET;
  if (i < 0 || i >= MAX_NUM_FILDES)
  {
    return -1;
  }
  if (entries[i].data == NULL || entries[i].type != FILDES_RAM)
  {
    return -1;
  }
  return i;
}

int createRamFileDescriptor(void)
{
  int i;

  for (i = 0; i < MAX_NUM_FILDES; i++)
  {
    if (entries[i].data == NULL)
    {
      RamFile *file = (RamFile *)Heap_calloc(1, sizeof(RamFile));
      logAssert(file);
      entries[i].data = (void *)file;
      entries[i].type = FILDES_RAM;
      return i + FILDES_OFFSET;
    }
  }

  return -1;
}

void destroyRamFileDescriptor(int fd)
{
  int i = indexFromRamFileDescriptor(fd);
  if (i < 0)
    return;
  Heap_free(entries[i].data);
  entries[i].data = NULL;
}

RamFile *lookupRamFileDescriptor(int fd)
{
  int i = indexFromRamFileDescriptor(fd);
  if (i < 0)
    return NULL;
  return (RamFile *)(entries[i].data);
}

bool isRamFileDescriptor(int fd)
{
  return indexFromRamFileDescriptor(fd) >= 0;
}
