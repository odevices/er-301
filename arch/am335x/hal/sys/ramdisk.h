#pragma once

#include <stddef.h>

#ifdef __cplusplus
extern "C"
{
#endif

  const char *ramdisk_lookup(const char *path, size_t *nbytes);
  const char *ramdisk_lookup_as_string(const char *path);

#ifdef __cplusplus
}
#endif
