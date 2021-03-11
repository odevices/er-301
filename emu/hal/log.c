#include <hal/log.h>
#include <hal/timing.h>
#include <hal/uart.h>
#include <hal/usb.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <hal/breakpoint.h>
#include <emu/tls.h>
#include <SDL2/SDL.h>

#define LOG_FMT_FILE_LINE "\033[36m[%s:%d]\033[0m"
#define LOG_FMT_FUNCTION "\033[34m%s()\033[0m"
#define LOG_FMT_PREFIX "\033[34m[#%d %0.4fs %s]\033[0m"

static int count = 0;

static const char *thread()
{
  const char *name = TLS_getName();
  if (name)
  {
    return name;
  }
  else
  {
    return "???";
  }
}

static const char *skipRelativePaths(const char *path)
{
  while (*path > 0 && (*path == '.' || *path == '/'))
  {
    path++;
  }
  return path;
}

static void flushNice()
{
  // TODO: make this nice
  fflush(stdout);
}

void Log_init()
{
}

int logCount()
{
  return count++;
}

void logWrite(const char *buffer, int len)
{
  Uart_write(buffer, len);
}

void _logSkipped()
{
  count++;
}

void _logRaw(const char *fmt, ...)
{
  va_list args;
  va_start(args, fmt);
  vprintf(fmt, args);
  va_end(args);
  flushNice();
}

void _logAssert(const char *filename, const char *function, int linenum, int expr)
{
  if (expr)
  {
    return;
  }
  filename = skipRelativePaths(filename);
  printf(LOG_FMT_PREFIX " \033[31;1mASSERT\033[0m " LOG_FMT_FUNCTION " " LOG_FMT_FILE_LINE "\n",
         count++, wallclock(), thread(), function, filename, linenum);
  fflush(stdout);
  Breakpoint();
}

void _logBriefNice(const char *label, const char *fmt, ...)
{
  char buffer[256];
  va_list args;
  va_start(args, fmt);
  vsnprintf(buffer, sizeof(buffer), fmt, args);
  va_end(args);
  printf(LOG_FMT_PREFIX " %s %s\n", count++, wallclock(), thread(), label, buffer);
  flushNice();
}

void _logCompleteNice(const char *label,
                      const char *filename,
                      const char *function,
                      int linenum,
                      const char *fmt, ...)
{
  char buffer[256];
  va_list args;
  va_start(args, fmt);
  vsnprintf(buffer, sizeof(buffer), fmt, args);
  va_end(args);
  filename = skipRelativePaths(filename);
  printf(LOG_FMT_PREFIX " %s " LOG_FMT_FUNCTION " %s " LOG_FMT_FILE_LINE "\n",
         count++, wallclock(), thread(), label, function, buffer, filename, linenum);
  flushNice();
}

void _logRude(const char *label,
              const char *filename,
              const char *function,
              int linenum,
              const char *fmt, ...)
{
  char buffer[256];
  va_list args;
  va_start(args, fmt);
  vsnprintf(buffer, sizeof(buffer), fmt, args);
  va_end(args);
  filename = skipRelativePaths(filename);
  printf(LOG_FMT_PREFIX " %s " LOG_FMT_FUNCTION " %s " LOG_FMT_FILE_LINE "\n",
         count++, wallclock(), thread(), label, function, buffer, filename, linenum);
  fflush(stdout);
  Breakpoint();
}
