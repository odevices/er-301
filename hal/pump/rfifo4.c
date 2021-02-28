#include <hal/log.h>
#include <hal/constants.h>
#include "rfifo4.h"
#include <malloc.h>

void rfifo4_alloc(rfifo4_t *fifo, int length)
{
  logAssert(fifo != NULL);
  logAssert(length > 0);
  fifo->wpos = 0;
  fifo->numread = length;
  int bytes = length * 2 * sizeof(float32x4_t);
  fifo->buffer = (float32x4_t *)memalign(CACHELINE_SIZE_MAX, bytes);
  logAssert(fifo->buffer);
  bzero(fifo->buffer, bytes);
  fifo->length = length;
  fifo->buffer2 = fifo->buffer + length;
}

void rfifo4_allocWithBuffer(rfifo4_t *fifo, float32x4_t *buffer, int length)
{
  logAssert(fifo != NULL);
  logAssert(length > 0);
  fifo->wpos = 0;
  fifo->numread = length;
  fifo->buffer = buffer;
  logAssert(fifo->buffer);
  fifo->length = length;
  fifo->buffer2 = fifo->buffer + length;
}

void rfifo4_free(rfifo4_t *fifo)
{
  free(fifo->buffer);
}
