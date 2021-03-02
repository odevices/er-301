#include <hal/fileops.h>
#include <sys/stat.h>
#include <sys/param.h>
#include <sys/mount.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <stdio.h>

bool createDirectory(const char *path)
{
  return mkdir(path, 0777) == 0;
}

bool deleteDirectory(const char *path)
{
  return rmdir(path) == 0;
}

bool moveDirectory(const char *fromPath, const char *toPath)
{
  return rename(fromPath, toPath) == 0;
}

bool deleteFile(const char *path)
{
  return unlink(path) == 0;
}

bool pathExists(const char *path)
{
  struct stat finfo;
  return stat(path, &finfo) == 0;
}

bool getFileInfo(const char *path, uint32_t *attributes, uint64_t *sizeInBytes)
{
  struct stat finfo;
  if (stat(path, &finfo) == 0)
  {
    if (attributes)
    {
      *attributes = 0;
      if (S_ISDIR(finfo.st_mode))
      {
        *attributes |= FILEOPS_DIR;
      }
      else if (!S_ISREG(finfo.st_mode))
      {
        *attributes |= FILEOPS_SYS;
      }
    }

    if (sizeInBytes)
    {
      *sizeInBytes = finfo.st_size;
    }

    return true;
  }

  return false;
}

bool isDirectory(const char *path)
{
  struct stat finfo;
  if (stat(path, &finfo) == 0)
  {
    return S_ISDIR(finfo.st_mode);
  }
  return false;
}

bool moveFile(const char *fromPath, const char *toPath, bool overwrite)
{
  if (overwrite && pathExists(toPath))
  {
    deleteFile(toPath);
  }
  return rename(fromPath, toPath) == 0;
}

static int cp(const char *from, const char *to)
{
  int fd_to, fd_from;
  char buf[4096];
  ssize_t nread;
  int saved_errno;

  fd_from = open(from, O_RDONLY);
  if (fd_from < 0)
    return -1;

  fd_to = open(to, O_WRONLY | O_CREAT | O_EXCL, 0666);
  if (fd_to < 0)
    goto out_error;

  while (nread = read(fd_from, buf, sizeof buf), nread > 0)
  {
    char *out_ptr = buf;
    ssize_t nwritten;

    do
    {
      nwritten = write(fd_to, out_ptr, nread);

      if (nwritten >= 0)
      {
        nread -= nwritten;
        out_ptr += nwritten;
      }
      else if (errno != EINTR)
      {
        goto out_error;
      }
    } while (nread > 0);
  }

  if (nread == 0)
  {
    if (close(fd_to) < 0)
    {
      fd_to = -1;
      goto out_error;
    }
    close(fd_from);

    /* Success! */
    return 0;
  }

out_error:
  saved_errno = errno;

  close(fd_from);
  if (fd_to >= 0)
    close(fd_to);

  errno = saved_errno;
  return -1;
}

bool copyFile(const char *fromPath, const char *toPath, bool overwrite)
{
  if (overwrite && pathExists(toPath))
  {
    deleteFile(toPath);
  }

  return cp(fromPath, toPath) == 0;
}

int diskFreeSpaceMB(const char *path)
{
  struct statfs fsinfo;
  if (statfs(path, &fsinfo) == 0)
  {
    uint64_t bytes = fsinfo.f_bsize * fsinfo.f_blocks;
    return (int)(bytes / 1024 / 1024);
  }
  return 0;
}

int diskTotalSpaceMB(const char *path)
{
  struct statfs fsinfo;
  if (statfs(path, &fsinfo) == 0)
  {
    uint64_t bytes = fsinfo.f_bsize * fsinfo.f_bavail;
    return (int)(bytes / 1024 / 1024);
  }
  return 0;
}
