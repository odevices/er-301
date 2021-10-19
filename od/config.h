#pragma once

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>
#include <stdbool.h>
#include <hal/constants.h>

  typedef struct
  {
    const char *filename;
    const char *frontRoot;
    const char *rearRoot;
    const char *xRoot;
    int frameLength;
    int sampleRate;     // in Hz
    int frameRate;      // in Hz
    float samplePeriod; // in secs
    float framePeriod;  // in secs
    float inputGain[NUM_INPUT_CHANNELS];
  } ConfigData;

  extern ConfigData globalConfig;

// Important: Use FRAMELENGTH instead of globalConfig.frameLength everywhere that speed matters.
//
// Frame lengths should already be multiples of 4 (for SIMD) but this macro 
// convinces the compiler of that fact so it can vectorize loops without the 
// post-processing step for non-multiples of 4.
#define FRAMELENGTH ((int)(4 * (globalConfig.frameLength / 4)))

  void Config_init(const char *filename, const char *xRoot, const char *rearRoot, const char *frontRoot);
  // Stored values are used in the next boot.
  bool Config_store(int sampleRate, int frameLength);

#ifdef __cplusplus
}
#endif