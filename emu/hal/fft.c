#include <hal/fft.h>
#include <hal/heap.h>
#include <hal/log.h>
#include <fftw3.h>

typedef struct
{
  fftwf_plan forward;
  fftwf_plan backward;
  int n;
} pimpl_t;

handle_rfft_t RFFT_allocate(int n)
{
  pimpl_t *pimpl = (pimpl_t *)Heap_calloc(1, sizeof(pimpl_t));
  pimpl->n = n;
  int m = n / 2 + 1;
  float *in = (float *)fftwf_malloc(n * sizeof(float));
  fftwf_complex *out = (fftwf_complex *)fftwf_malloc(m * sizeof(fftwf_complex));

  pimpl->forward = fftwf_plan_dft_r2c_1d(n, in, out, FFTW_ESTIMATE);
  if (pimpl->forward == NULL)
  {
    logError("Failed to create forward plan.");
  }

  pimpl->backward = fftwf_plan_dft_c2r_1d(n, out, in, FFTW_ESTIMATE);
  if (pimpl->backward == NULL)
  {
    logError("Failed to create forward plan.");
  }

  fftwf_free(in);
  fftwf_free(out);
  return (handle_rfft_t)pimpl;
}

void RFFT_destroy(handle_rfft_t handle)
{
  pimpl_t *pimpl = (pimpl_t *)handle;
  fftwf_destroy_plan(pimpl->forward);
  fftwf_destroy_plan(pimpl->backward);
  Heap_free(pimpl);
}

void RFFT_forward(complex_float_t *dst, float *src, handle_rfft_t handle)
{
  pimpl_t *pimpl = (pimpl_t *)handle;
  fftwf_execute_dft_r2c(pimpl->forward, src, (fftwf_complex *)dst);
}

void RFFT_inverse(float *dst, complex_float_t *src, handle_rfft_t handle)
{
  pimpl_t *pimpl = (pimpl_t *)handle;
  fftwf_execute_dft_c2r(pimpl->backward, (fftwf_complex *)src, dst);
  // FFTW inverse is scaled by n.
  float scale = 1.0f / pimpl->n;
  for (int i = 0; i < 4 * (pimpl->n / 4); i++)
  {
    dst[i] *= scale;
  }
}

handle_cfft_t CFFT_allocate(int n)
{
  pimpl_t *pimpl = (pimpl_t *)Heap_calloc(1, sizeof(pimpl_t));
  pimpl->n = n;
  fftwf_complex *in = (fftwf_complex *)fftwf_malloc(n * sizeof(fftwf_complex));
  fftwf_complex *out = (fftwf_complex *)fftwf_malloc(n * sizeof(fftwf_complex));

  pimpl->forward = fftwf_plan_dft_1d(n, in, out, FFTW_FORWARD, FFTW_ESTIMATE);
  if (pimpl->forward == NULL)
  {
    logError("Failed to create forward plan.");
  }

  pimpl->backward = fftwf_plan_dft_1d(n, out, in, FFTW_BACKWARD, FFTW_ESTIMATE);
  if (pimpl->backward == NULL)
  {
    logError("Failed to create forward plan.");
  }

  fftwf_free(in);
  fftwf_free(out);
  return (handle_rfft_t)pimpl;
}

void CFFT_destroy(handle_cfft_t handle)
{
  pimpl_t *pimpl = (pimpl_t *)handle;
  fftwf_destroy_plan(pimpl->forward);
  fftwf_destroy_plan(pimpl->backward);
  Heap_free(pimpl);
}

void CFFT_forward(complex_float_t *dst, complex_float_t *src, handle_cfft_t handle)
{
  pimpl_t *pimpl = (pimpl_t *)handle;
  fftwf_execute_dft(pimpl->forward, (fftwf_complex *)src, (fftwf_complex *)dst);
}

void CFFT_inverse(complex_float_t *dst, complex_float_t *src, handle_cfft_t handle)
{
  pimpl_t *pimpl = (pimpl_t *)handle;
  fftwf_execute_dft(pimpl->backward, (fftwf_complex *)src, (fftwf_complex *)dst);
  // FFTW inverse is scaled by n.
  float scale = 1.0f / pimpl->n;
  float * out = (float *)dst;
  for (int i = 0; i < 8 * (pimpl->n / 4); i++)
  {
    out[i] *= scale;
  }
}
