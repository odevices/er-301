#pragma once

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

  typedef void *dir_t;

  dir_t Dir_open(const char *path);
  void Dir_close(dir_t dir);
  bool Dir_read(dir_t dir, char **filename, uint32_t *attributes);

#ifdef __cplusplus
}
#endif