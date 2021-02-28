#pragma once

#include <hal/constants.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

#define MAIN_HORIZONTAL_PIXELS 256
#define MAIN_VERTICAL_PIXELS 64
#define MAIN_PIXELS_PER_SHORT 2

#define SUB_HORIZONTAL_PIXELS 128
#define SUB_VERTICAL_PIXELS 64
#define SUB_PIXELS_PER_SHORT 8

// sizeof(uint16_t) is to account for the LCDC LIDD bus width (upper byte of each uint16_t is ignored)
#define MAIN_FRAME_BUFFER_BYTES (sizeof(uint16_t) * (MAIN_HORIZONTAL_PIXELS * MAIN_VERTICAL_PIXELS) / MAIN_PIXELS_PER_SHORT)
#define SUB_FRAME_BUFFER_BYTES (sizeof(uint16_t) * (SUB_HORIZONTAL_PIXELS * SUB_VERTICAL_PIXELS) / SUB_PIXELS_PER_SHORT)

  typedef struct
  {
    uint8_t main[CACHE_ALIGNED_SIZE(MAIN_FRAME_BUFFER_BYTES)] __attribute__((aligned(CACHELINE_SIZE_MAX)));
    uint8_t sub[CACHE_ALIGNED_SIZE(SUB_FRAME_BUFFER_BYTES)] __attribute__((aligned(CACHELINE_SIZE_MAX)));
  } DisplayBuffer;

  void Display_init(void);
  void Display_deinit();
  void Display_start(void);
  void Display_stop(void);
  DisplayBuffer *Display_getBuffer();
  void Display_putBuffer(DisplayBuffer *frame);

  // returns last buffer that was given to Display_putBuffer()
  DisplayBuffer *Display_getLastPutBuffer();

#ifdef __cplusplus
}
#endif
