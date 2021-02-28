#include <od/extras/ComplexFFT.h>

namespace od
{

  ComplexFFT::ComplexFFT()
  {
  }

  ComplexFFT::ComplexFFT(int n)
  {
    allocate(n);
  }

  ComplexFFT::~ComplexFFT()
  {
    destroy();
  }

  void ComplexFFT::allocate(int n)
  {
    destroy();
    mHandle = CFFT_allocate(n);
  }

  void ComplexFFT::destroy()
  {
    if (mHandle)
    {
      CFFT_destroy(mHandle);
      mHandle = 0;
    }
  }

  void ComplexFFT::compute(complex_float_t *fout, complex_float_t *fin)
  {
    CFFT_forward(fout, fin, mHandle);
  }

  void ComplexFFT::computeInverse(complex_float_t *fout,
                                  complex_float_t *fin)
  {
    CFFT_inverse(fout, fin, mHandle);
  }

} /* namespace od */
