#include <string.h>
#include "reflect.h"

struct sym_table_t gbl_sym_table[1] __attribute__((weak)) = {{0, 0}};

uintptr_t reflect_query_symbol(const char *name)
{
  struct sym_table_t *p = &gbl_sym_table[0];

  for (; p->name; p++)
  {
    if (strcmp(p->name, name) == 0)
    {
      return p->addr;
    }
  }
  return 0;
}