#include <hal/priorities.h>
#include <hal/gpio.h>
#include <hal/pwm.h>
#include <hal/constants.h>
#include <string.h>

typedef struct
{
  int red[PWM_NUM_CHANNELS];
  int green[PWM_NUM_CHANNELS];
  uint16_t data[PWM_NUM_CHANNELS * 2];
} pwm_t;

static pwm_t self;

void Pwm_set(int channel, float value)
{
  value *= 2.0f;
  if (value > 0)
  {
    if (value < 1.0f)
    {
      self.data[self.green[channel]] = (uint16_t)(value * 0xFFF);
      self.data[self.red[channel]] = 0;
    }
    else
    {
      self.data[self.green[channel]] = (uint16_t)(0xFFF);
      self.data[self.red[channel]] = 0;
    }
  }
  else
  {
    if (value > -1.0f)
    {
      self.data[self.green[channel]] = 0;
      self.data[self.red[channel]] = (uint16_t)(-value * 0xFFF);
    }
    else
    {
      self.data[self.green[channel]] = 0;
      self.data[self.red[channel]] = (uint16_t)(-0xFFF);
    }
  }
}

void Pwm_set_raw(int channel, float red, float green)
{
  self.data[self.red[channel]] = (uint16_t)(red * 0xFFF);
  self.data[self.green[channel]] = (uint16_t)(green * 0xFFF);
}

void Pwm_start(void)
{

}

void Pwm_init(void)
{
  int i;

  for (i = 0; i < PWM_NUM_CHANNELS; i++)
  {
    self.data[2 * i] = 0;
    self.data[2 * i + 1] = 0;
    self.red[i] = 2 * i;
    self.green[i] = 2 * i + 1;
  }
}
