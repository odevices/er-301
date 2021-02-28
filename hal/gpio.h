#pragma once

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C"
{
#endif

#ifndef SWIGLUA
  typedef void (*GpioCallback)(uint32_t id, uint32_t value);
  void Gpio_init(void);
  void Gpio_setEvents(uint32_t id, uint32_t up, uint32_t dn);
#endif

  bool Gpio_read(uint32_t id);
  void Gpio_write(uint32_t id, bool value);
  void Gpio_toggle(uint32_t id);

  static void inline Led_on(uint32_t id)
  {
    Gpio_write(id, true);
  }

  static void inline Led_off(uint32_t id)
  {
    Gpio_write(id, false);
  }

  static void inline Led_toggle(uint32_t id)
  {
    Gpio_toggle(id);
  }

  static inline bool Button_pressed(uint32_t id)
  {
    return !Gpio_read(id);
  }

// GPIO ids
#define LED_SAFE 0
#define LED_IO 1
#define LED_DIAL1 2
#define LED_DIAL2 3
#define LED_OUT1 4
#define LED_OUT2 5
#define LED_OUT3 6
#define LED_OUT4 7
#define LED_LINK12 8
#define LED_LINK34 9
#define BUTTON_MAIN1 10
#define BUTTON_MAIN2 11
#define BUTTON_MAIN3 12
#define BUTTON_MAIN4 13
#define BUTTON_MAIN5 14
#define BUTTON_MAIN6 15
#define BUTTON_SUB1 16
#define BUTTON_SUB2 17
#define BUTTON_SUB3 18
#define BUTTON_ENTER 19
#define BUTTON_UP 20
#define BUTTON_SHIFT 21
#define BUTTON_DIAL1 22
#define BUTTON_DIAL2 23
#define BUTTON_DIAL3 24
#define BUTTON_SELECT1 25
#define BUTTON_SELECT2 26
#define BUTTON_SELECT3 27
#define BUTTON_SELECT4 28
#define TOGGLE_STORAGE_A 29
#define TOGGLE_STORAGE_B 30
#define TOGGLE_MODE_A 31
#define TOGGLE_MODE_B 32
#define PWM_SIN 33
#define PWM_SCLK 34
#define PWM_XLAT 35
#define PWM_BLANK 36
#define MAIN_OLED_RESET 37
#define SUB_OLED_RESET 38
#define OLED_POWER 39
#define PCM4104_nRESET 40
#define PCM4104_MUTE 41
#define ADS8688_RESET 42
#define AUDIO_EXTERNAL_CLOCK_ENABLE 43
#define LED_LINK23 44
#define PCM4104_FS0 45
#define PCM4104_FS1 46
#define NUM_GPIO_IDS 47

#ifdef __cplusplus
}
#endif
