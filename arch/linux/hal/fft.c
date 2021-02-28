#include <hal/fft.h>

handle_rfft_t RFFT_allocate(int n)
{
  return 0;
}

void RFFT_destroy(handle_rfft_t handle)
{
}

void RFFT_forward(complex_float_t *dst, float *src, handle_rfft_t handle)
{
}

void RFFT_inverse(float *dst, complex_float_t *src, handle_rfft_t handle)
{
}

handle_cfft_t CFFT_allocate(int n)
{
  return 0;
}

void CFFT_destroy(handle_cfft_t handle)
{
}

void CFFT_forward(complex_float_t *dst, complex_float_t *src, handle_cfft_t handle)
{
}

void CFFT_inverse(complex_float_t *dst, complex_float_t *src, handle_cfft_t handle)
{
}
