#include <hal/dir.h>
//#define BUILDOPT_VERBOSE
//#define BUILDOPT_DEBUG_LEVEL 10
#include <hal/log.h>
#include <hal/fileops.h>
#include <dirent.h>
#include <string.h>

dir_t Dir_open(const char *path)
{
  logDebug(1, path);
  return (dir_t)opendir(path);
}

void Dir_close(dir_t dir)
{
  DIR *d = (DIR *)dir;
  logAssert(d);
  closedir(d);
}

bool Dir_read(dir_t dir, char **filename, uint32_t *attributes)
{
  DIR *d = (DIR *)dir;
  logAssert(d);

  struct dirent *entry = readdir(d);

  // Skip the . and .. files.
  while (entry && ((strcmp(entry->d_name, ".") == 0) || (strcmp(entry->d_name, "..") == 0)))
  {
    logDebug(1, "Skipping %s", entry->d_name);
    entry = readdir(d);
  }

// Skip anything that is not a directory or a regular file.
#define SKIP(x) (((x & DT_DIR) == 0) && ((x & DT_REG) == 0))
  while (entry && SKIP(entry->d_type))
  {
    logDebug(1, "Skipping %s (d_type = 0x%x)", entry->d_name, entry->d_type);
    entry = readdir(d);
  }

  if (entry)
  {
    if (filename)
    {
      *filename = entry->d_name;
    }

    if (attributes)
    {
      *attributes = 0;
      if (entry->d_type == DT_DIR)
      {
        *attributes |= FILEOPS_DIR;
      }
    }
  }

  return entry != NULL;
}