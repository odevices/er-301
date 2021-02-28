#include <hal/rng.h>
#include <hal/log.h>
#include <stdio.h>

void Rng_init(void)
{
}

uint64_t Rng_read64(void)
{
  uint64_t output;
  FILE *f = fopen("/dev/urandom", "r");
  if (f)
  {
    if (fread(&output, sizeof(uint64_t), 1, f) != 1)
    {
      logError("Could not read from /dev/urandom.");
    }
    fclose(f);
  }
  else
  {
    logError("Could not open /dev/urandom.");
  }
  return output;
}

uint32_t Rng_read32(void)
{
  uint32_t output;
  FILE *f = fopen("/dev/urandom", "r");
  if (f)
  {
    if (fread(&output, sizeof(uint32_t), 1, f) != 1)
    {
      logError("Could not read from /dev/urandom.");
    }
    fclose(f);
  }
  else
  {
    logError("Could not open /dev/urandom.");
  }
  return output;
}
