#include <hal/fft.h>
#include <NE10.h>

handle_rfft_t RFFT_allocate(int n)
{
  return ne10_fft_alloc_r2c_float32(n);
}

void RFFT_destroy(handle_rfft_t handle)
{
  ne10_fft_destroy_r2c_float32((ne10_fft_r2c_cfg_float32_t)handle);
}

void RFFT_forward(complex_float_t *dst, float *src, handle_rfft_t handle)
{
  ne10_fft_r2c_1d_float32_neon((ne10_fft_cpx_float32_t *)dst,
                               src,
                               (ne10_fft_r2c_cfg_float32_t)handle);
}

void RFFT_inverse(float *dst, complex_float_t *src, handle_rfft_t handle)
{
  ne10_fft_c2r_1d_float32_neon(dst,
                               (ne10_fft_cpx_float32_t *)src,
                               (ne10_fft_r2c_cfg_float32_t)handle);
}

handle_cfft_t CFFT_allocate(int n)
{
  return ne10_fft_alloc_c2c_float32_neon(n);
}

void CFFT_destroy(handle_cfft_t handle)
{
  ne10_fft_destroy_c2c_float32((ne10_fft_cfg_float32_t)handle);
}

void CFFT_forward(complex_float_t *dst, complex_float_t *src, handle_cfft_t handle)
{
  ne10_fft_c2c_1d_float32_neon((ne10_fft_cpx_float32_t *)dst,
                               (ne10_fft_cpx_float32_t *)src,
                               (ne10_fft_cfg_float32_t)handle, 0);
}

void CFFT_inverse(complex_float_t *dst, complex_float_t *src, handle_cfft_t handle)
{
  ne10_fft_c2c_1d_float32_neon((ne10_fft_cpx_float32_t *)dst,
                               (ne10_fft_cpx_float32_t *)src,
                               (ne10_fft_cfg_float32_t)handle, 1);
}
