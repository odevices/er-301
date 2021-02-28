#pragma once

//// WeakRB bounded FIFO queue
// Ref: Correct and Efficient Bounded FIFO Queues
// https://www.irif.fr/~guatto/papers/sbac13.pdf
// Ported from C11 atomics to GCC atomics.
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#define FIFO_SIZE 128

typedef struct
{
  size_t front, pfront;
  size_t back, cback;
  uint32_t data[FIFO_SIZE];
} fifo_t;

static inline void fifo_init(fifo_t *fifo)
{
  fifo->pfront = fifo->front = 0;
  fifo->cback = fifo->back = 0;
}

static inline bool fifo_push(fifo_t *fifo, uint32_t value)
{
  size_t b;
  b = __atomic_load_n(&fifo->back, __ATOMIC_RELAXED);
  if (fifo->pfront + FIFO_SIZE - b < 1)
  {
    fifo->pfront = __atomic_load_n(&fifo->front, __ATOMIC_ACQUIRE);
    if (fifo->pfront + FIFO_SIZE - b < 1)
    {
      return false;
    }
  }
  fifo->data[b % FIFO_SIZE] = value;
  __atomic_store_n(&fifo->back, b + 1, __ATOMIC_RELEASE);
  return true;
}

static inline bool fifo_pop(fifo_t *fifo, uint32_t *value)
{
  size_t f;
  f = __atomic_load_n(&fifo->front, __ATOMIC_RELAXED);
  if (fifo->cback - f < 1)
  {
    fifo->cback = __atomic_load_n(&fifo->back, __ATOMIC_ACQUIRE);
    if (fifo->cback - f < 1)
    {
      return false;
    }
  }
  *value = fifo->data[f % FIFO_SIZE];
  __atomic_store_n(&fifo->front, f + 1, __ATOMIC_RELEASE);
  return true;
}

static inline bool fifo_is_empty(fifo_t *fifo)
{
  size_t f;
  f = __atomic_load_n(&fifo->front, __ATOMIC_RELAXED);
  if (fifo->cback - f < 1)
  {
    fifo->cback = __atomic_load_n(&fifo->back, __ATOMIC_ACQUIRE);
    if (fifo->cback - f < 1)
    {
      return true;
    }
  }
  return false;
}
