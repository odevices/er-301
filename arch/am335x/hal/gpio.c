#include <stdint.h>
#include <stdbool.h>
#include <ti/am335x/hw_types.h>
#include <ti/am335x/soc.h>
#include <ti/am335x/hw_cm_per.h>
#include <ti/am335x/hw_cm_wkup.h>
#include <ti/am335x/gpio_lld.h>
#include <ti/am335x/hw_gpio.h>
#include <hal/priorities.h>
#include <hal/constants.h>
#include <hal/log.h>
#include <hal/gpio.h>
#include <hal/events.h>
#include <ti/sysbios/family/arm/a8/intcps/Hwi.h>

#define GPIO_INVALID_ID 0xFF
#define GPIO_INVALID_PIN 0xFF
#define GPIO_INVALID_PORT 0xFF

typedef struct
{
  uint32_t up_event;
  uint32_t dn_event;
  uint8_t port;
  uint8_t pin;
  bool cached_value;
} gpio_t;

typedef struct
{
  uint8_t pin2id[4][32];
  gpio_t gpio[NUM_GPIO_IDS];
} Local;

static Local local;

static uint32_t baseAddress(uint32_t port)
{
  switch (port)
  {
  case 0:
    return SOC_GPIO_0_REGS;
  case 1:
    return SOC_GPIO_1_REGS;
  case 2:
    return SOC_GPIO_2_REGS;
  case 3:
    return SOC_GPIO_3_REGS;
  default:
    return 0;
  }
}

static bool readPin(uint32_t id)
{
  return GPIOPinRead(baseAddress(local.gpio[id].port), local.gpio[id].pin) > 0;
}

static void configureOutput(uint32_t id, uint32_t port, uint32_t pin, uint32_t value)
{
  logAssert(local.pin2id[port][pin] == GPIO_INVALID_ID);
  local.pin2id[port][pin] = id;
  local.gpio[id].port = (uint8_t)port;
  local.gpio[id].pin = (uint8_t)pin;
  local.gpio[id].up_event = EVENT_NONE;
  local.gpio[id].dn_event = EVENT_NONE;
  local.gpio[id].cached_value = false;
  Gpio_write(id, value);
  GPIOSetDirMode(baseAddress(port), pin, GPIO_DIRECTION_OUTPUT);
}

static void configureSwitch(uint32_t id, uint32_t port, uint32_t pin)
{
  uint32_t base = baseAddress(port);
  logAssert(local.pin2id[port][pin] == GPIO_INVALID_ID);
  GPIOSetDirMode(base, pin, GPIO_DIRECTION_INPUT);
  GPIODebounceFuncEnable(base, pin, true);
  GPIOSetIntrType(base, pin, GPIO_INTR_MASK_BOTH_EDGE);
  GPIOIntrClear(base, GPIO_INTR_LINE_1, pin);
  GPIOIntrDisable(base, GPIO_INTR_LINE_1, pin);
  local.pin2id[port][pin] = id;
  local.gpio[id].port = (uint8_t)port;
  local.gpio[id].pin = (uint8_t)pin;
  local.gpio[id].up_event = EVENT_NONE;
  local.gpio[id].dn_event = EVENT_NONE;
  local.gpio[id].cached_value = readPin(id);
}

static void configureButton(uint32_t id, uint32_t port, uint32_t pin)
{
  uint32_t base = baseAddress(port);
  logAssert(local.pin2id[port][pin] == GPIO_INVALID_ID);
  GPIOSetDirMode(base, pin, GPIO_DIRECTION_INPUT);
  GPIODebounceFuncEnable(base, pin, 1);
  GPIOSetIntrType(base, pin, GPIO_INTR_MASK_BOTH_EDGE);
  GPIOIntrClear(base, GPIO_INTR_LINE_1, pin);
  GPIOIntrDisable(base, GPIO_INTR_LINE_1, pin);
  local.pin2id[port][pin] = id;
  local.gpio[id].port = (uint8_t)port;
  local.gpio[id].pin = (uint8_t)pin;
  local.gpio[id].up_event = EVENT_NONE;
  local.gpio[id].dn_event = EVENT_NONE;
  local.gpio[id].cached_value = readPin(id);
}

void Gpio_setEvents(uint32_t id, uint32_t up, uint32_t dn)
{
  local.gpio[id].up_event = up;
  local.gpio[id].dn_event = dn;

  if (up != EVENT_NONE || dn != EVENT_NONE)
  {
    GPIOIntrEnable(baseAddress(local.gpio[id].port), GPIO_INTR_LINE_1, local.gpio[id].pin);
  }
  else
  {
    GPIOIntrDisable(baseAddress(local.gpio[id].port), GPIO_INTR_LINE_1, local.gpio[id].pin);
  }
}

static inline uint32_t getLowestSetBit(uint32_t x)
{
  uint32_t idx = 0;

  while (!(x & 0x1))
  {
    idx++;
    x = x >> 1;
  }

  return idx;
}

static void update(uint32_t id, bool value)
{
  bool up = (!local.gpio[id].cached_value) && value;
  bool dn = local.gpio[id].cached_value && (!value);
  local.gpio[id].cached_value = value;

  if (up && local.gpio[id].up_event != EVENT_NONE)
  {
    Events_push(local.gpio[id].up_event);
  }
  else if (dn && local.gpio[id].dn_event != EVENT_NONE)
  {
    Events_push(local.gpio[id].dn_event);
  }
}

static void hwiOnInterrupt(UArg arg)
{
  uint32_t port = (uint32_t)arg;
  uint32_t base = baseAddress(port);
  uint32_t status;
  uint32_t pin;
  uint32_t data;

  /* Find out which pins have their interrupt flags set */
  status = GPIOIntrRawStatus(base, GPIO_INTR_LINE_1, GPIO_PIN_MASK_ALL);
  data = HW_RD_REG32(base + GPIO_DATAIN);

  /* Clear all the set bits at once */
  GPIOIntrClearMask(base, GPIO_INTR_LINE_1, status);

  while (status)
  {
    /* Gets the lowest order set bit number */
    pin = getLowestSetBit(status);
    status &= ~(1 << pin);
    update(local.pin2id[port][pin], (data & (1 << pin)) > 0);
  }
}

static void configure_port(uint32_t port)
{
  uint32_t intrpt;

  /* Enable the GPIO module. */
  GPIOModuleEnable(baseAddress(port), true);

  // Lookup the interrupt line and enable the debounce clock
  switch (port)
  {
  case 0:
    intrpt = GPIO0_1_INT;
    HW_WR_FIELD32_RAW(
        SOC_CM_WKUP_REGS + CM_WKUP_GPIO0_CLKCTRL,
        CM_WKUP_GPIO0_CLKCTRL_OPTFCLKEN_GPIO0_GDBCLK,
        CM_WKUP_GPIO0_CLKCTRL_OPTFCLKEN_GPIO0_GDBCLK_SHIFT,
        CM_WKUP_GPIO0_CLKCTRL_OPTFCLKEN_GPIO0_GDBCLK_FCLK_EN);
    break;
  case 1:
    intrpt = GPIO1_1_INT;
    HW_WR_FIELD32_RAW(
        SOC_CM_PER_REGS + CM_PER_GPIO1_CLKCTRL,
        CM_PER_GPIO1_CLKCTRL_OPTFCLKEN_GPIO_1_GDBCLK,
        CM_PER_GPIO1_CLKCTRL_OPTFCLKEN_GPIO_1_GDBCLK_SHIFT,
        CM_PER_GPIO1_CLKCTRL_OPTFCLKEN_GPIO_1_GDBCLK_FCLK_EN);
    break;
  case 2:
    intrpt = GPIO2_1_INT;
    HW_WR_FIELD32_RAW(
        SOC_CM_PER_REGS + CM_PER_GPIO2_CLKCTRL,
        CM_PER_GPIO2_CLKCTRL_OPTFCLKEN_GPIO_2_GDBCLK,
        CM_PER_GPIO2_CLKCTRL_OPTFCLKEN_GPIO_2_GDBCLK_SHIFT,
        CM_PER_GPIO2_CLKCTRL_OPTFCLKEN_GPIO_2_GDBCLK_FCLK_EN);
    break;
  case 3:
    intrpt = GPIO3_1_INT;
    HW_WR_FIELD32_RAW(
        SOC_CM_PER_REGS + CM_PER_GPIO3_CLKCTRL,
        CM_PER_GPIO3_CLKCTRL_OPTFCLKEN_GPIO_3_GDBCLK,
        CM_PER_GPIO3_CLKCTRL_OPTFCLKEN_GPIO_3_GDBCLK_SHIFT,
        CM_PER_GPIO3_CLKCTRL_OPTFCLKEN_GPIO_3_GDBCLK_FCLK_EN);
    break;
  default:
    return;
  }

  // Assign interrupt handler
  Hwi_Params params;
  Hwi_Params_init(&params);
  params.priority = HWI_PRIORITY_GPIO;
  params.arg = port;

  Hwi_create(intrpt, hwiOnInterrupt, &params, NULL);

  int i;
  for (i = 0; i < 32; i++)
  {
    local.pin2id[port][i] = GPIO_INVALID_ID;
  }

  // set debounce time in cycles of the 32kHz debounce clock (i.e. 31us)
  // (D6C datasheet claims <10ms)
  // 0-255
  GPIOSetDebounceTime(baseAddress(port), 255); // 8ms
}

bool Gpio_read(uint32_t id)
{
  return local.gpio[id].cached_value;
}

void Gpio_write(uint32_t id, bool value)
{
  GPIOPinWrite(baseAddress(local.gpio[id].port), local.gpio[id].pin, value);
  update(id, value);
}

void Gpio_toggle(uint32_t id)
{
  Gpio_write(id, Gpio_read(id) ? false : true);
}

void Gpio_init(void)
{
  for (int i = 0; i < NUM_GPIO_IDS; i++)
  {
    local.gpio[i].up_event = EVENT_NONE;
    local.gpio[i].dn_event = EVENT_NONE;
    local.gpio[i].port = GPIO_INVALID_PORT;
    local.gpio[i].pin = GPIO_INVALID_PIN;
    local.gpio[i].cached_value = false;
  }

  configure_port(0);
  configure_port(1);
  configure_port(2);
  configure_port(3);

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
