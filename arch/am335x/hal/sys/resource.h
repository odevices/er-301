#pragma once

#include <hal/fatfs/ff.h>
#include <stdbool.h>

#define FILE_READ_CACHE_ENABLED 1
#define FILE_READ_CACHE_LEN (32 * 1024)

#if FILE_READ_CACHE_ENABLED
typedef struct
{
  int offset;
  int len;
  char *data;
} cache_t;
#endif

typedef struct
{
  FIL fil;
#if FILE_READ_CACHE_ENABLED
  int pos;
  cache_t cache;
#endif
} file_t;

bool isCardFileDescriptor(int fd);
int createCardFileDescriptor(void);
void destroyCardFileDescriptor(int fd);
file_t *lookupCardFileDescriptor(int fd);

typedef struct
{
  const char *begin;
  const char *current;
  uint32_t nbytes;
} RamFile;

bool isRamFileDescriptor(int fd);
int createRamFileDescriptor(void);
void destroyRamFileDescriptor(int fd);
RamFile *lookupRamFileDescriptor(int fd);
