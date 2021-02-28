#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

typedef struct
{
  bool enabled;
  bool initialized;
} Local;

static Local self = {
    .enabled = false,
    .initialized = false,
};

void Uart_init(void)
{
  self.initialized = true;
}

void Uart_write(const char *buffer, int size)
{
  if (!self.enabled)
    return;
#if 0
  for (int i = 0; i < size; i++)
  {
    if (buffer[i])
    {
      if (buffer[i] == '\n')
      {
        fputc('\r', stdout);
      }
      fputc(buffer[i], stdout);
    }
  }
#else
  fwrite(buffer, 1, size, stdout);
#endif
  fflush(stdout);
}

void Uart_puts(const char *buffer)
{
  if (!self.enabled)
    return;
  fputs(buffer, stdout);
  fflush(stdout);
}

void Uart_putc(char c)
{
  fputc(c, stdout);
  fflush(stdout);
}

void Uart_enable()
{
  self.enabled = self.initialized;
}

void Uart_disable()
{
  self.enabled = false;
}

bool Uart_isEnabled()
{
  return self.enabled;
}
