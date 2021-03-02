
#include <emu/emu.h>
#include <hal/log.h>
#include <stdlib.h>
#include <stdio.h>
#include <limits.h>

int main(int argc, char *args[])
{
  if (argc < 4)
  {
    logRaw("Usage: %s <path-to-xroot> <path-to-rear-root> <path-to-front-root>", args[0]);
    return -1;
  }
  else
  {
    // Expand relative paths to absolute paths
    char xroot[PATH_MAX];
    char rear[PATH_MAX];
    char front[PATH_MAX];
    realpath(args[1], xroot);
    realpath(args[2], rear);
    realpath(args[3], front);
    return emu::run(xroot, rear, front);
  }
}
