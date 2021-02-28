#include "ElfFile.h"
#include "dlfcn.h"
#define BUILDOPT_VERBOSE
#define BUILDOPT_DEBUG_LEVEL 10
#include <hal/log.h>
#include <map>
#include <string>

enum ErrorCodes
{
  NONE = 0,
  UNSUPPORTED_MODE,
  ELF_LOAD_FAILED,
};

struct Local
{
  ErrorCodes mLastError = NONE;
  std::map<std::string, od::ElfFile *> mLoaded;
};

static Local local;

int dlclose(void *__handle)
{
  od::ElfFile *elf = (od::ElfFile *)__handle;
  logDebug(1, "closing %s", elf->path().c_str());
  if (elf->refCount() == 1)
  {
    local.mLoaded.erase(elf->path());
  }
  elf->release();
  return 0;
}

const char *dlerror(void)
{
  const char *msg;
  switch (local.mLastError)
  {
  case UNSUPPORTED_MODE:
    msg = "Mode for dlopen() must be RTLD_NOW | RTLD_LOCAL.";
    break;
  case ELF_LOAD_FAILED:
    msg = "Failed to load ELF file.";
    break;
  case NONE:
  default:
    msg = "No error.";
    break;
  }
  local.mLastError = NONE;
  return msg;
}

void *dlopen(const char *__path, int __mode)
{
  logDebug(1, "opening %s", __path);
  if (__mode != (RTLD_NOW | RTLD_LOCAL))
  {
    local.mLastError = UNSUPPORTED_MODE;
    return NULL;
  }
  auto i = local.mLoaded.find(__path);
  if (i != local.mLoaded.end())
  {
    // already loaded
    od::ElfFile *elf = i->second;
    logDebug(1, "already loaded %s", elf->path().c_str());
    elf->attach();
    return elf;
  }
  od::ElfFile *elf = new od::ElfFile();
  if (!elf->load(__path))
  {
    local.mLastError = ELF_LOAD_FAILED;
    delete elf;
    return NULL;
  }
  elf->attach();
  local.mLoaded[__path] = elf;
  return elf;
}

void *dlsym(void *__handle, const char *__symbol)
{
  od::ElfFile *elf = (od::ElfFile *)__handle;
  return elf->lookup(__symbol);
}
