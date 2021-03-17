#pragma once

#include <hal/fft.h>
#include <od/extras/AlignmentAllocator.h>
#include <vector>

namespace od
{

  typedef std::vector<float, AlignmentAllocator<float>> RealBuffer;
  typedef std::vector<complex_float_t, AlignmentAllocator<complex_float_t>> ComplexBuffer;

  class RealFFT
  {
  public:
    RealFFT();
    RealFFT(int n);
    virtual ~RealFFT();

    void allocate(int n);
    void destroy();

    // complex (n/2+1) from real (n)
    void compute(complex_float_t *fout, float *fin);
    // real (n) from complex (n/2+1)
    void computeInverse(float *fout, complex_float_t *fin);

  private:
    handle_rfft_t mHandle = 0;
  };

} /* namespace od */
