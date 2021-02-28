#pragma once

#ifdef __cplusplus
extern "C"
{
#endif

  typedef struct
  {
    float r;
    float i;
  } complex_float_t;

  typedef void *handle_rfft_t;
  typedef void *handle_cfft_t;

  handle_rfft_t RFFT_allocate(int n);
  void RFFT_destroy(handle_rfft_t handle);
  void RFFT_forward(complex_float_t *dst, float *src, handle_rfft_t handle);
  void RFFT_inverse(float *dst, complex_float_t *src, handle_rfft_t handle);

  handle_cfft_t CFFT_allocate(int n);
  void CFFT_destroy(handle_cfft_t handle);
  void CFFT_forward(complex_float_t *dst, complex_float_t *src, handle_cfft_t handle);
  void CFFT_inverse(complex_float_t *dst, complex_float_t *src, handle_cfft_t handle);

#ifdef __cplusplus
}
#endif