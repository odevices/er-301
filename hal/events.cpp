#include <hal/gpio.h>
#include <hal/constants.h>
#include <hal/fifo.h>
#include <hal/events.h>
#include <hal/encoder.h>
#include <hal/concurrency/EventFlags.h>
//#define BUILDOPT_VERBOSE
//#define BUILDOPT_DEBUG_LEVEL 10
#include <hal/log.h>

#define REPEAT_PERIOD 3
#define REPEAT_DELAY 25

typedef struct
{
  fifo_t Q;
  od::EventFlags events;
#define onPost od::EventFlags::flag00
  int buttonTimer[19] = {0};
  int lastEncoderValue = 0;
} Local;

static Local local;

static void checkButtonRepeat(uint32_t id)
{
  int i = id - BUTTON_MAIN1;
  if (Button_pressed(id))
  {
    if (local.buttonTimer[i] > REPEAT_PERIOD + REPEAT_DELAY)
    {
      local.buttonTimer[i] = REPEAT_DELAY;
      Events_push(EVENT(EVENT_REPEAT, id));
    }
    else
    {
      local.buttonTimer[i]++;
    }    
  }
  else
  {
    local.buttonTimer[i] = 0;
  }
}

static void checkEncoder()
{
  int value = Encoder_getValue();
  if (value != local.lastEncoderValue)
  {
    Events_push(EVENT_KNOB);
    local.lastEncoderValue = value;
  }
}

static void configureButtonEvents(uint32_t id)
{
  Gpio_setEvents(id, EVENT(EVENT_RELEASE, id), EVENT(EVENT_PRESS, id));
}

//////////////////

extern "C"
{

  void Events_init(void)
  {
    fifo_init(&local.Q);

    configureButtonEvents(BUTTON_MAIN1);
    configureButtonEvents(BUTTON_MAIN2);
    configureButtonEvents(BUTTON_MAIN3);
    configureButtonEvents(BUTTON_MAIN4);
    configureButtonEvents(BUTTON_MAIN5);
    configureButtonEvents(BUTTON_MAIN6);
    configureButtonEvents(BUTTON_SUB1);
    configureButtonEvents(BUTTON_SUB2);
    configureButtonEvents(BUTTON_SUB3);
    configureButtonEvents(BUTTON_ENTER);
    configureButtonEvents(BUTTON_UP);
    configureButtonEvents(BUTTON_SHIFT);
    configureButtonEvents(BUTTON_DIAL1);
    configureButtonEvents(BUTTON_DIAL2);
    configureButtonEvents(BUTTON_DIAL3);
    configureButtonEvents(BUTTON_SELECT1);
    configureButtonEvents(BUTTON_SELECT2);
    configureButtonEvents(BUTTON_SELECT3);
    configureButtonEvents(BUTTON_SELECT4);
    Gpio_setEvents(TOGGLE_MODE_A, EVENT_MODE, EVENT_MODE);
    Gpio_setEvents(TOGGLE_MODE_B, EVENT_MODE, EVENT_MODE);
    Gpio_setEvents(TOGGLE_STORAGE_A, EVENT_STORAGE, EVENT_STORAGE);
    Gpio_setEvents(TOGGLE_STORAGE_B, EVENT_STORAGE, EVENT_STORAGE);

    local.lastEncoderValue = Encoder_getValue();
  }

  void Events_push(uint32_t e)
  {
    logDebug(1,"push type=%d id=%d", EVENT_TYPE(e), EVENT_ID(e));
    fifo_push(&local.Q, e);
    local.events.post(onPost);
  }

  void Events_clear(void)
  {
    fifo_init(&local.Q);
  }

  void Events_wait(void)
  {
    checkButtonRepeat(BUTTON_MAIN1);
    checkButtonRepeat(BUTTON_MAIN2);
    checkButtonRepeat(BUTTON_MAIN3);
    checkButtonRepeat(BUTTON_MAIN4);
    checkButtonRepeat(BUTTON_MAIN5);
    checkButtonRepeat(BUTTON_MAIN6);
    checkButtonRepeat(BUTTON_SUB1);
    checkButtonRepeat(BUTTON_SUB2);
    checkButtonRepeat(BUTTON_SUB3);
    checkButtonRepeat(BUTTON_ENTER);
    checkButtonRepeat(BUTTON_UP);
    checkButtonRepeat(BUTTON_SHIFT);
    checkButtonRepeat(BUTTON_DIAL1);
    checkButtonRepeat(BUTTON_DIAL2);
    checkButtonRepeat(BUTTON_DIAL3);
    checkButtonRepeat(BUTTON_SELECT1);
    checkButtonRepeat(BUTTON_SELECT2);
    checkButtonRepeat(BUTTON_SELECT3);
    checkButtonRepeat(BUTTON_SELECT4);
    checkEncoder();
    local.events.waitForAll(onPost);
  }

  uint32_t Events_pull(void)
  {
    uint32_t value = EVENT_NONE;
    fifo_pop(&local.Q, &value);
    return value;
  }
}