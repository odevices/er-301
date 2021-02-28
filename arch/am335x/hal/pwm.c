#include <hal/priorities.h>
#include <hal/gpio.h>
#include <hal/pwm.h>
#include <hal/constants.h>

#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Task.h>

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

static void transmit_data(void)
{
  int ch, bit;

  Gpio_write(PWM_XLAT, 0);
  for (ch = 0; ch < PWM_NUM_CHANNELS * 2; ch++)
  {
    uint16_t tmp = self.data[ch];
    for (bit = 11; bit >= 0; bit--)
    {
      Gpio_write(PWM_SCLK, 0);
      Gpio_write(PWM_SIN, (tmp >> bit) & 0x1);
      Gpio_write(PWM_SCLK, 1);
    }
  }
  Gpio_write(PWM_XLAT, 1);
}

static Void pwm_task(UArg arg0, UArg arg1)
{
  transmit_data();
  Gpio_write(PWM_BLANK, 0);

  while (1)
  {
    transmit_data();
    Task_sleep(18);
  }
}

void Pwm_start(void)
{
  Task_Params params;

  Task_Params_init(&params);
  params.priority = TASK_PRIORITY_PWM;
  params.env = (Ptr)"pwm";
  Task_create(pwm_task, &params, NULL);
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
