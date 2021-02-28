#include <hal/modulation.h>
#include <hal/channels.h>
#include <string.h>

static struct
{
  uint32_t range0[MOD_NUM_CHANNELS_PER_ADC];
  uint32_t range1[MOD_NUM_CHANNELS_PER_ADC];
} local;

void Modulation_init()
{
  memset(&local, 0, sizeof(local));
}

void Modulation_start(void)
{
}

void Modulation_restart(void)
{
}

void Modulation_setChannelRange(uint32_t channel, uint32_t range)
{
  if (channel >= MOD_NUM_CHANNELS)
    return;

  if (channel < MOD_NUM_CHANNELS_PER_ADC)
  {
    local.range0[channel] = range;
  }
  else
  {
    channel -= MOD_NUM_CHANNELS_PER_ADC;
    local.range1[channel] = range;
  }
}

uint32_t Modulation_getChannelRange(uint32_t channel)
{
  if (channel >= MOD_NUM_CHANNELS)
    return MOD_BIPOLAR_2500;

  if (channel < MOD_NUM_CHANNELS_PER_ADC)
  {
    return local.range0[channel];
  }
  else
  {
    channel -= MOD_NUM_CHANNELS_PER_ADC;
    return local.range1[channel];
  }
}
