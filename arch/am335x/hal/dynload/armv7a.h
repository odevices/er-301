#ifndef __dynload_armv7a_H__
#define __dynload_armv7a_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "elf.h"
#include <stdint.h>
#include <stdbool.h>

bool verifyHeader(Elf32_Ehdr * header);
bool relocate(const Elf32_Rel *rel, const Elf32_Sym *sym, uintptr_t addr, const char *symbolName);

#ifdef __cplusplus
}
#endif

#endif // __dynload_armv7a_H__
