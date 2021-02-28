#include <od/extras/RealFFT.h>

namespace od
{

  RealFFT::RealFFT()
  {
  }

  RealFFT::RealFFT(int n)
  {
    allocate(n);
  }

  RealFFT::~RealFFT()
  {
    destroy();
  }

  void RealFFT::allocate(int n)
  {
    destroy();
    mHandle = RFFT_allocate(n);
  }

  void RealFFT::destroy()
  {
    if (mHandle)
    {
      RFFT_destroy(mHandle);
      mHandle = 0;
    }
  }

  // real to complex
  void RealFFT::compute(complex_float_t *fout, float *fin)
  {
    RFFT_forward(fout, fin, mHandle);
  }

  // complex to real
  void RealFFT::computeInverse(float *fout, complex_float_t *fin)
  {
    RFFT_inverse(fout, fin, mHandle);
  }

} /* namespace od */
