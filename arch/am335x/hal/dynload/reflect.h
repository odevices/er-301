#pragma once

#include <stdint.h>

struct sym_table_t
{
  const char *name;
  uintptr_t addr;
};

uintptr_t reflect_query_symbol(const char *name);
