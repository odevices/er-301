#include <hal/timing.h>
#include <hal/log.h>
#include <SDL2/SDL_timer.h>

typedef struct
{
  uint64_t offset;
} Local;

static Local local;

tick_t ticks(void)
{
  return SDL_GetPerformanceCounter();
}

double ticks2secsD(tick_t nticks)
{
  uint64_t f = SDL_GetPerformanceFrequency();
  double p = 1.0 / f;
  return nticks * p;
}

float ticks2secs(tick_t nticks)
{
  uint64_t f = SDL_GetPerformanceFrequency();
  float p = 1.0 / f;
  return nticks * p;
}

float wallclock(void)
{
  uint64_t t = SDL_GetPerformanceCounter() - local.offset;
  uint64_t f = SDL_GetPerformanceFrequency();
  float p = 1.0 / f;
  return t * p;
}

void wallclock_reset(void)
{
  local.offset = SDL_GetPerformanceCounter();
}

unsigned long micros(void)
{
  return (SDL_GetPerformanceCounter() / (SDL_GetPerformanceFrequency() / 1000000));
}

void Timing_init(void)
{
  logInfo("--------Timing Init--------------");
  wallclock_reset();
  logInfo("Tick Frequency: %llu Hz", SDL_GetPerformanceFrequency());
  logInfo("Tick Period: %d ns", (int)(ticks2secsD(1) * 1000000000));
  logInfo("---------------------------------");
}
