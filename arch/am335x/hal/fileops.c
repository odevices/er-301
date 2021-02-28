#include <hal/fileops.h>
#include <hal/fatfs/ff.h>

bool createDirectory(const char *path)
{
  return f_mkdir(path) == FR_OK;
}

bool deleteDirectory(const char *path)
{
  return f_unlink(path) == FR_OK;
}

bool moveDirectory(const char *fromPath, const char *toPath)
{
  return f_rename(fromPath, toPath) == FR_OK;
}

bool deleteFile(const char *path)
{
  return f_unlink(path) == FR_OK;
}

bool pathExists(const char *path)
{
  FILINFO finfo;
  return f_stat(path, &finfo) == FR_OK;
}

bool getFileInfo(const char *path, uint32_t *attributes, uint64_t *sizeInBytes)
{
  FILINFO finfo;
  if (f_stat(path, &finfo) == FR_OK)
  {
    if (attributes)
    {
      *attributes = finfo.fattrib;
    }

    if (sizeInBytes)
    {
      *sizeInBytes = finfo.fsize;
    }

    return true;
  }

  return false;
}

bool isDirectory(const char *path)
{
  DIR dir;
  if (f_opendir(&dir, path) == FR_OK)
  {
    f_closedir(&dir);
    return true;
  }

  return false;
}

bool moveFile(const char *fromPath, const char *toPath, bool overwrite)
{
  if (overwrite && pathExists(toPath))
  {
    deleteFile(toPath);
  }
  return f_rename(fromPath, toPath) == FR_OK;
}

bool copyFile(const char *fromPath, const char *toPath, bool overwrite)
{
  if (overwrite && pathExists(toPath))
  {
    deleteFile(toPath);
  }

  FIL src, dst;
  if (f_open(&src, fromPath, FA_READ) != FR_OK)
  {
    return false;
  }

  if (f_open(&dst, toPath, FA_WRITE | FA_CREATE_NEW) != FR_OK)
  {
    f_close(&src);
    return false;
  }

  char buffer[1024];
  while (true)
  {
    UINT read, written;
    if (f_read(&src, buffer, sizeof(buffer), &read) != FR_OK)
    {
      break;
    }
    if (read > 0)
    {
      if (f_write(&dst, buffer, read, &written) != FR_OK)
      {
        break;
      }
    }
    else
    {
      break;
    }
  }

  f_close(&src);
  f_close(&dst);
  return true;
}

int diskFreeSpaceMB(const char *path)
{
  FATFS *fs;
  DWORD fre_clust, fre_sect;

  /* Get volume information and free clusters of drive 1 */
  FRESULT res = f_getfree(path, &fre_clust, &fs);
  if (res)
    return 0;

  fre_sect = fre_clust * fs->csize;
  return fre_sect / 2 / 1024;
}

int diskTotalSpaceMB(const char *path)
{
  FATFS *fs;
  DWORD fre_clust, tot_sect;

  /* Get volume information and free clusters of drive 1 */
  FRESULT res = f_getfree(path, &fre_clust, &fs);
  if (res)
    return 0;

  /* Get total sectors and free sectors */
  tot_sect = (fs->n_fatent - 2) * fs->csize;

  return tot_sect / 2 / 1024;
}
