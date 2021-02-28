#include <stdint.h>
#include <stdbool.h>
#include <hal/priorities.h>
#include <hal/constants.h>
#include <hal/log.h>
#include <hal/gpio.h>
#include <hal/events.h>

typedef struct
{
  uint32_t up_event;
  uint32_t dn_event;
  bool value;
} gpio_t;

typedef struct
{
  gpio_t info[NUM_GPIO_IDS];
} Local;

static Local local;

static void configureOutput(uint32_t id, uint32_t port, uint32_t pin, uint32_t value)
{
  local.info[id].up_event = EVENT_NONE;
  local.info[id].dn_event = EVENT_NONE;
  local.info[id].value = false;
}

static void configureSwitch(uint32_t id, uint32_t port, uint32_t pin)
{
  local.info[id].up_event = EVENT_NONE;
  local.info[id].dn_event = EVENT_NONE;
  local.info[id].value = false;
}

static void configureButton(uint32_t id, uint32_t port, uint32_t pin)
{
  local.info[id].up_event = EVENT_NONE;
  local.info[id].dn_event = EVENT_NONE;
  local.info[id].value = true;
}

void Gpio_setEvents(uint32_t id, uint32_t up, uint32_t dn)
{
  local.info[id].up_event = up;
  local.info[id].dn_event = dn;
}

bool Gpio_read(uint32_t id)
{
  return local.info[id].value;
}

void Gpio_write(uint32_t id, bool value)
{
  bool up = (!local.info[id].value) && value;
  bool dn = local.info[id].value && (!value);
  local.info[id].value = value;

  if (up && local.info[id].up_event != EVENT_NONE)
  {
    Events_push(local.info[id].up_event);
  }
  else if (dn && local.info[id].dn_event != EVENT_NONE)
  {
    Events_push(local.info[id].dn_event);
  }
}

void Gpio_toggle(uint32_t id)
{
  Gpio_write(id, Gpio_read(id) ? false : true);
}

void Gpio_init(void)
{
  int i;
  for (i = 0; i < NUM_GPIO_IDS; i++)
  {
    local.info[i].up_event = EVENT_NONE;
    local.info[i].dn_event = EVENT_NONE;
    local.info[i].value = false;
  }

  configureOutput(LED_SAFE, 0, 27, 0);
  configureOutput(LED_IO, 0, 22, 0);
  configureOutput(LED_DIAL1, 0, 26, 0);
  configureOutput(LED_DIAL2, 1, 15, 0);
  configureOutput(LED_OUT1, 3, 4, 0);
  configureOutput(LED_OUT2, 0, 18, 0);
  configureOutput(LED_OUT3, 0, 21, 0);
  configureOutput(LED_OUT4, 1, 21, 0);
  configureOutput(LED_LINK12, 2, 20, 0);
  configureOutput(LED_LINK34, 0, 28, 0);
  configureOutput(LED_LINK23, 3, 3, 0);

  configureOutput(PWM_SIN, 0, 8, 0);
  configureOutput(PWM_SCLK, 0, 9, 0);
  configureOutput(PWM_XLAT, 0, 10, 0);
  configureOutput(PWM_BLANK, 0, 31, 1);

  configureOutput(MAIN_OLED_RESET, 3, 17, 0);
  configureOutput(SUB_OLED_RESET, 1, 25, 0);
  configureOutput(OLED_POWER, 3, 21, 0);

  configureOutput(PCM4104_nRESET, 0, 29, 0);
  configureOutput(PCM4104_MUTE, 0, 0, 1);

  // switched from rev 6 onwards
  configureOutput(PCM4104_FS0, 3, 0, 0);
  configureOutput(PCM4104_FS1, 0, 12, 0);
  ///////////

  configureOutput(AUDIO_EXTERNAL_CLOCK_ENABLE, 3, 2, 0);

  configureOutput(ADS8688_RESET, 1, 23, 0);

  configureButton(BUTTON_MAIN1, 0, 7);
  configureButton(BUTTON_MAIN2, 0, 20);
  configureButton(BUTTON_MAIN3, 3, 5);
  configureButton(BUTTON_MAIN4, 0, 14);
  configureButton(BUTTON_MAIN5, 0, 1);
  configureButton(BUTTON_MAIN6, 0, 16);

  configureButton(BUTTON_SUB1, 1, 7);
  configureButton(BUTTON_SUB2, 2, 0);
  configureButton(BUTTON_SUB3, 1, 17);
  configureButton(BUTTON_ENTER, 1, 6);
  configureButton(BUTTON_UP, 1, 16);
  configureButton(BUTTON_SHIFT, 1, 18);

  configureButton(BUTTON_DIAL1, 0, 23);
  configureButton(BUTTON_DIAL2, 1, 13);
  configureButton(BUTTON_DIAL3, 2, 17);

  configureButton(BUTTON_SELECT1, 2, 19);
  configureButton(BUTTON_SELECT2, 2, 21);
  configureButton(BUTTON_SELECT3, 3, 13);
  configureButton(BUTTON_SELECT4, 1, 22);

  configureSwitch(TOGGLE_STORAGE_A, 2, 14);
  configureSwitch(TOGGLE_STORAGE_B, 1, 12);

  configureSwitch(TOGGLE_MODE_A, 2, 16);
  configureSwitch(TOGGLE_MODE_B, 1, 14);
}
