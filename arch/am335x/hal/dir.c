#include <hal/dir.h>
#include <hal/fatfs/ff.h>
#include <hal/log.h>
#include <malloc.h>

typedef struct
{
  DIR dir;
  FILINFO finfo;
} internal_dir_t;

dir_t Dir_open(const char *path)
{
  internal_dir_t *d = (internal_dir_t *)calloc(1, sizeof(internal_dir_t));
  logAssert(d);
  if (f_opendir(&d->dir, path) != FR_OK)
  {
    return 0;
  }
  return d;
}

void Dir_close(dir_t dir)
{
  internal_dir_t *d = (internal_dir_t *)dir;
  logAssert(d);
  f_closedir(&d->dir);
  free(d);
}

bool Dir_read(dir_t dir, char **filename, uint32_t *attributes)
{
  internal_dir_t *d = (internal_dir_t *)dir;
  logAssert(d);

  FRESULT res;
  res = f_readdir(&d->dir, &d->finfo);
  if (res != FR_OK || d->finfo.fname[0] == 0)
  { /* Break on error or end of dir */
    return false;
  }

  if (filename)
  {
    *filename = d->finfo.fname;
  }

  if (attributes)
  {
    *attributes = d->finfo.fattrib;
  }

  return true;
}