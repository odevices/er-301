#pragma once

#include <hal/simd.h>
#include <string.h>

#ifdef __cplusplus
extern "C"
{
#endif

  typedef struct
  {
    float32x4_t *buffer;
    float32x4_t *buffer2;
    int length;
    int wpos;
    int numread;
  } rfifo4_t;

  void rfifo4_alloc(rfifo4_t *fifo, int length);
  void rfifo4_allocWithBuffer(rfifo4_t *fifo, float32x4_t *buffer, int length);
  void rfifo4_free(rfifo4_t *fifo);

  static inline int rfifo4_count(rfifo4_t *fifo)
  {
    return fifo->length - fifo->numread;
  }

  static inline void rfifo4_pop(rfifo4_t *fifo, int n)
  {
    fifo->numread += n;
    if (fifo->numread > fifo->length)
      fifo->numread = fifo->length;
  }

  static inline float32x4_t *rfifo4_get(rfifo4_t *fifo)
  {
    return fifo->buffer + fifo->wpos + fifo->numread;
  }

  static inline void rfifo4_push1(rfifo4_t *fifo, float32x4_t value)
  {
    if (fifo->wpos < fifo->length)
    {
      fifo->buffer[fifo->wpos] = value;
      fifo->buffer2[fifo->wpos] = value;
      fifo->wpos++;
    }
    else
    {
      fifo->buffer[0] = value;
      fifo->buffer2[0] = value;
      fifo->wpos = 1;
    }
    if (fifo->numread > 0)
      fifo->numread--;
  }

  static inline void float32x4copy(float32x4_t *dst, const float32x4_t *src, uint32_t n)
  {
    memcpy(dst, src, n * sizeof(float32x4_t));
  }

  static inline void rfifo4_push(rfifo4_t *fifo, float32x4_t *buffer, int n)
  {
    // can write fifo much before wrapping
    int remaining = fifo->length - fifo->wpos;

    if (n > fifo->length) // too much data
    {
      buffer += n - fifo->length;
      n = fifo->length;
    }

    if (n <= remaining)
    {
      float32x4copy(fifo->buffer + fifo->wpos, buffer, n);
      float32x4copy(fifo->buffer2 + fifo->wpos, buffer, n);
      fifo->wpos += n;
    }
    else
    {
      float32x4copy(fifo->buffer + fifo->wpos, buffer, remaining);
      float32x4copy(fifo->buffer2 + fifo->wpos, buffer, remaining);
      // writeIndex is now zero
      remaining = n - remaining;
      float32x4copy(fifo->buffer, buffer, remaining);
      float32x4copy(fifo->buffer2, buffer, remaining);
      fifo->wpos = remaining;
    }

    if (fifo->numread > n)
      fifo->numread -= n;
    else
      fifo->numread = 0;
  }

  static inline void rfifo4_eatn(rfifo4_t *fifo, float32x4_t *out, int n, int stride)
  {
    uint32_t remaining;
    float32x4_t *last = rfifo4_get(fifo);
    float32x4_t *end = out + n * stride;
    for (; out < end; out += stride)
    {
      remaining = rfifo4_count(fifo);
      if (remaining == 1)
      {
        rfifo4_push1(fifo, last[remaining - 1]);
      }
      last = rfifo4_get(fifo);
      *out = last[0];
      rfifo4_pop(fifo, 1);
    }
  }

#ifdef __cplusplus
}
#endif
