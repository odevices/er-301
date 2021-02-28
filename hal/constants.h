#pragma once

#ifdef __cplusplus
extern "C"
{
#endif

#define CACHELINE_SIZE_MAX (64U)
#define CACHE_ALIGNED_SIZE(nbytes) ((((nbytes) + CACHELINE_SIZE_MAX - 1) / CACHELINE_SIZE_MAX) * CACHELINE_SIZE_MAX)

#define NUM_OUTPUT_CHANNELS 4
#define NUM_INPUT_CHANNELS 20

// rate=96kHz
// 32 samples - drop outs at 900 (in audio task) CPU 40%
// 64 samples - drop outs at 1400 MFLOPS
// 128 samples - drop outs at 1700 MFLOPS
// 256 samples - drop outs at 1800 MFLOPS
// DAC latency: 29 samples (Group delay, p5 of PCM4104 datasheet)
// rate=48kHz
// 32 samples - drop outs at CPU 65%, 2.6ms round-trip latency
// 64 samples - drop outs at CPU 70%,
// 128 samples - drop outs at CPU 80%, 7ms round-trip latency
#define MAX_AUDIO_FRAME_LENGTH (128)
#define ADC_FRAME_LENGTH (32)
#define MAX_ADC_FRAME_LENGTH (32)
#define SPI_FRAME_LENGTH (32)
#define MAX_SPI_FRAME_LENGTH (32)

#define GRAPHICS_REFRESH_RATE 55.0f
#define GRAPHICS_REFRESH_PERIOD (1.0f / GRAPHICS_REFRESH_RATE)

#define FULLSCALE_IN_VOLTS (10.0f)

// Just in case NULL is not defined.
#ifndef NULL
#define NULL (0)
#endif

#ifdef __cplusplus
}
#endif
