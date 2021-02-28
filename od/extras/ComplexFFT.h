#pragma once

#include <hal/fft.h>

namespace od
{

  class ComplexFFT
  {
  public:
    ComplexFFT();
    ComplexFFT(int n);
    virtual ~ComplexFFT();

    void allocate(int n);
    void destroy();

    // complex to complex
    void compute(complex_float_t *fout, complex_float_t *fin);
    void computeInverse(complex_float_t *fout, complex_float_t *fin);

  private:
    handle_cfft_t mHandle = 0;
  };

} /* namespace od */
