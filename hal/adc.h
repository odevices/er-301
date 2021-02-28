#pragma once

#ifdef __cplusplus
extern "C"
{
#endif

#define ADC_NUM_CHANNELS 4

#define ADC_VOLTAGE_MAX (10.0f)
#define ADC_CODE_MAX (0xFFF)
#define ADC_HEADROOM (1.0f)

  void Adc_init();
  void Adc_start(void);
  void Adc_stop(void);

  // Called on each frame in the ADC thread.
  extern void Adc_callback(int *samples);

#ifdef __cplusplus
}
#endif
