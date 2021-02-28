#pragma once

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

  bool createDirectory(const char *path);
  bool deleteDirectory(const char *path);
  bool moveDirectory(const char *fromPath, const char *toPath);

  bool deleteFile(const char *path);
  bool moveFile(const char *fromPath, const char *toPath, bool overwrite);
  bool copyFile(const char *fromPath, const char *toPath, bool overwrite);

  bool pathExists(const char *path);
  bool isDirectory(const char *path);

  int diskTotalSpaceMB(const char *path);
  int diskFreeSpaceMB(const char *path);

  bool getFileInfo(const char *path, uint32_t *attributes, uint64_t *sizeInBytes);

#define FILEOPS_RDO 0x01 /* Read only */
#define FILEOPS_HID 0x02 /* Hidden */
#define FILEOPS_SYS 0x04 /* System */
#define FILEOPS_DIR 0x10 /* Directory */
#define FILEOPS_ARC 0x20 /* Archive */

#ifdef __cplusplus
}
#endif
