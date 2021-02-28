#include <hal/rng.h>
#include <stdbool.h>

typedef volatile struct rng *rng_ref_t;

struct rng
{
  /*00*/ uint64_t output;     //r-
  /*08*/ uint32_t status;     //r-
  /*0c*/ uint32_t irq_en;     //rw
  /*10*/ uint32_t status_clr; //-c
  /*14*/ uint32_t control;    //rw
  /*18*/ uint32_t config;     //rw
};

#define RNG_BASE 0x48310000

// power domain PER
// `- clock domain L4LS  (enabled by default)
//    `- module RNG
//
#define CM_PER_RNG_CLKCTRL 0x44e00090

rng_ref_t const rng = (rng_ref_t)RNG_BASE;

#define RNG_STATUS_RDY (1u << 0)  // output ready for reading
#define RNG_STATUS_ERR (1u << 1)  // FRO shutdown alarm
#define RNG_STATUS_CLK (1u << 31) // module functional clock active (no irq)

void Rng_init(void)
{
  static bool initialized = false;
  if (initialized)
    return;

  // enable module clock
  *(volatile uint8_t *)CM_PER_RNG_CLKCTRL = 2;
  while (*(volatile uint32_t *)CM_PER_RNG_CLKCTRL & 0x30000)
  {
  }

  rng->config = 0 | 34 << 16 // max refill 34 * 256 cycles
                | 33 << 0    // min refill 33 * 64 cycles
      ;
  rng->control = 0 | 33 << 16 // startup 33 * 256 cycles
                 | 1 << 10    // enable module
      ;

  initialized = true;
}

// Warning! Slow calls ahead.
// ticks per call = 21,000

uint64_t Rng_read64(void)
{
  do
  {
  } while ((rng->status & RNG_STATUS_RDY) == 0);
  uint64_t output = rng->output;
  rng->status_clr = RNG_STATUS_RDY;
  return output;
}

uint32_t Rng_read32(void)
{
  do
  {
  } while ((rng->status & RNG_STATUS_RDY) == 0);
  uint32_t output = (uint32_t)rng->output;
  rng->status_clr = RNG_STATUS_RDY;
  return output;
}
