
#include <emu/emu.h>
#include <hal/log.h>

int main(int argc, char *args[])
{
  if (argc < 4)
  {
    logError("Usage: %s <path-to-xroot> <path-to-rear-root> <path-to-front-root>", args[0]);
    return -1;
  }
  else
  {
    return emu::run(args[1], args[2], args[3]);
  }
}
