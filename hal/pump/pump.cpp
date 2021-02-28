#include <od/config.h>
#include <hal/card.h>
#include <hal/log.h>
#include <hal/adc.h>
#include <hal/modulation.h>
#include <hal/audio.h>
#include <hal/constants.h>
#include <hal/channels.h>
#include <hal/pump.h>
#include <hal/concurrency/Mutex.h>
#include "pidcontrol.h"
#include "resample4.h"

typedef struct _Local
{
  float output_gain;
  float adc_input_gain;

  rfifo4_t adc_fifo;
  resampler4_t R[4];
  od::Mutex mFifoMutex;

  float speed_ratio;
  uint32_t depthQ[8];
  uint32_t depthQi;
  PDController ratio_controller;

  float out_frame[MAX_AUDIO_FRAME_LENGTH * NUM_OUTPUT_CHANNELS] __attribute__((aligned(CACHELINE_SIZE_MAX)));
  float in_frame[MAX_AUDIO_FRAME_LENGTH * NUM_INPUT_CHANNELS] __attribute__((aligned(CACHELINE_SIZE_MAX)));

  int renderCallCount;
  int renderCallThrottle;
} Local;

static Local self;

void Adc_callback(int *samples)
{
  float32x4_t y, gain;

  gain = vdupq_n_f32(self.adc_input_gain);

  self.mFifoMutex.enter();
  int i;
  for (i = 0; i < ADC_NUM_CHANNELS * ADC_FRAME_LENGTH; i += 4)
  {
    y = vcvtq_f32_s32(vld1q_s32((int32_t *)(samples + i)));
    y = vmulq_f32(gain, y);
    rfifo4_push1(&self.adc_fifo, y);
  }
  self.mFifoMutex.leave();
}

void Mod0_callback(int *samples)
{
  int32x4_t offset;
  float32x4_t y, gain0, gain1;

  offset = vdupq_n_s32(-0x8000);
  gain0 = vld1q_f32(globalConfig.inputGain);
  gain1 = vld1q_f32(globalConfig.inputGain + 4);

  self.mFifoMutex.enter();
  int i;
  for (i = 0; i < MOD_NUM_CHANNELS_PER_ADC * SPI_FRAME_LENGTH; i += 8)
  {
    y = vcvtq_f32_s32(
        vaddq_s32(vld1q_s32((int32_t *)(samples + i)), offset));
    resampler4_write1(&self.R[0], vmulq_f32(gain0, y));

    y = vcvtq_f32_s32(
        vaddq_s32(vld1q_s32((int32_t *)(samples + i + 4)), offset));
    resampler4_write1(&self.R[1], vmulq_f32(gain1, y));
  }
  self.mFifoMutex.leave();
}

void Mod1_callback(int *samples)
{
  int32x4_t offset;
  float32x4_t y, gain2, gain3;

  offset = vdupq_n_s32(-0x8000);
  gain2 = vld1q_f32(globalConfig.inputGain + 8);
  gain3 = vld1q_f32(globalConfig.inputGain + 12);

  self.mFifoMutex.enter();
  int i;
  for (i = 0; i < MOD_NUM_CHANNELS_PER_ADC * SPI_FRAME_LENGTH; i += 8)
  {
    y = vcvtq_f32_s32(
        vaddq_s32(vld1q_s32((int32_t *)(samples + i)), offset));
    resampler4_write1(&self.R[2], vmulq_f32(gain2, y));

    y = vcvtq_f32_s32(
        vaddq_s32(vld1q_s32((int32_t *)(samples + i + 4)), offset));
    resampler4_write1(&self.R[3], vmulq_f32(gain3, y));
  }
  self.mFifoMutex.leave();
}

/*
static int ABCDx_depth = 0;
static int Gx_depth = 0;
void printFifoDepths()
{
    logInfo("ABCDx FIFO: %d samples", ABCDx_depth);
    logInfo("Gx FIFO: %d samples", Gx_depth);
}
*/

void Audio_callback(int *samples)
{
  int i;
  float32x4_t *neon = (float32x4_t *)self.in_frame;

#if 1
  // Filter the fifo depth and sample
  self.depthQ[self.depthQi++] = resampler4_count(&self.R[0]);
  if (self.depthQi == 8)
    self.depthQi = 0;
  // find minimum depth in the last 8 calls
  uint32_t depth = self.depthQ[0];
  if (depth > self.depthQ[1])
    depth = self.depthQ[1];
  if (depth > self.depthQ[2])
    depth = self.depthQ[2];
  if (depth > self.depthQ[3])
    depth = self.depthQ[3];
  if (depth > self.depthQ[4])
    depth = self.depthQ[4];
  if (depth > self.depthQ[5])
    depth = self.depthQ[5];
  if (depth > self.depthQ[6])
    depth = self.depthQ[6];
  if (depth > self.depthQ[7])
    depth = self.depthQ[7];

  // update PID controller for sampling ratio
  float ratioAdj = PDController_update(&self.ratio_controller, depth);
  float ratio = self.speed_ratio + ratioAdj;
#else
  static float depth = 0.0f;
  depth = 0.9999 * depth + 0.0001 * resampler4_count(&self.R[0]);

  // update PID controller for sampling ratio
  float ratioAdj = PDController_update(&self.ratio_controller, depth);
  float ratio = self.speed_ratio + ratioAdj;

#endif

  // in_frame: (mod0-3, mod4-7, mod8-11, mod12-15, adc0-3) x globalConfig.frameLength
  self.mFifoMutex.enter();
  // fill the in_frame with resampled mod samples
  for (i = 0; i < 4; i++)
  {
    resampler4_upSample(&self.R[i], neon + i, globalConfig.frameLength,
                           NUM_INPUT_CHANNELS / 4,
                           ratio);
  }

  // fill the in_frame with adc samples
  rfifo4_eatn(&self.adc_fifo, neon + 4, globalConfig.frameLength,
                NUM_INPUT_CHANNELS / 4);

  //ABCDx_depth = resampler4_count(&self.R[0]);
  //Gx_depth = rfifo4_count(&self.adc_fifo);

  //if(Gx_depth > globalConfig.frameLength * 0.1f) {
  //	rfifo4_pop(&self.adc_fifo,Gx_depth - globalConfig.frameLength * 0.1f);
  //}

  self.mFifoMutex.leave();

#if 0
    // observe PD state
    for (i = 0; i < globalConfig.frameLength * NUM_INPUT_CHANNELS; i +=
            NUM_INPUT_CHANNELS)
    {
        self.in_frame[i + INPUT_P1] = depth / (float) self.R[0].fifo.length;
        //self.in_frame[i + INPUT_P2] = depthQ[0] / (float) self.R[0].fifo.length;
        self.in_frame[i + INPUT_P3] = ratioAdj * 20;
        self.in_frame[i + INPUT_P4] = self.ratio_controller.e[0]
        / (float) self.R[0].fifo.length;
    }
#endif

  memset(self.out_frame, 0,
         sizeof(float) * NUM_OUTPUT_CHANNELS * FRAMELENGTH);

  if (self.renderCallCount < self.renderCallThrottle)
  {
    self.renderCallCount++;
    Pump_callback(self.in_frame, self.out_frame);
  }
  else
  {
    // Other tasks have been starved, so throttle signal processing.
  }

  // final hard-limiter, gain stage, and integer conversion
  float32x4_t x, gain = vdupq_n_f32(self.output_gain);
  float32x4_t minimum = vdupq_n_f32(-1.0f);
  float32x4_t maximum = vdupq_n_f32(1.0f);
  int n = FRAMELENGTH * NUM_OUTPUT_CHANNELS;
  for (i = 0; i < n; i += 4)
  {
    x = vmaxq_f32(vminq_f32(vld1q_f32(self.out_frame + i), maximum),
                  minimum);                                // hard-limit
    x = vmulq_f32(x, gain);                                // gain
    vst1q_s32((int32_t *)(samples + i), vcvtq_s32_f32(x)); // int conversion
  }
}

void Pump_resetThrottle()
{
  self.renderCallCount = 0;
}

void Pump_init()
{
  self.renderCallThrottle = globalConfig.frameRate * 0.1f;
  Pump_resetThrottle();

  // number of input (mod) samples consumed for each output sample
  self.speed_ratio = (float)MOD_SAMPLE_RATE / (float)globalConfig.sampleRate;
  self.output_gain = AUDIO_SAFE_MAX_OUTPUT_VALUE;

  self.adc_input_gain = ADC_HEADROOM / ADC_CODE_MAX;
  uint32_t n = (uint32_t)(globalConfig.frameLength * self.speed_ratio);
  resampler4_init(&self.R[0], 2 * n);
  resampler4_init(&self.R[1], 2 * n);
  resampler4_init(&self.R[2], 2 * n);
  resampler4_init(&self.R[3], 2 * n);
  rfifo4_alloc(&self.adc_fifo, 2 * globalConfig.frameLength);
  PDController_init(&self.ratio_controller, 1.25f * n,      // set point
                    (-1.0f / n) * globalConfig.framePeriod, // error gain
                    10.0f,                                  // integration time (in secs)
                    globalConfig.framePeriod,               // dt (in secs)
                    self.speed_ratio * 0.1f, self.speed_ratio * (-0.1f));
  memset(self.depthQ, 0, sizeof(self.depthQ));
  self.depthQi = 0;
}