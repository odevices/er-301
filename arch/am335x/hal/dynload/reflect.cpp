#include "reflect.h"

extern uintptr_t gbl_sym_table_lookup(const char *name) __attribute__((weak));

uintptr_t reflect_query_symbol(const char *name)
{
  return gbl_sym_table_lookup(name);
}