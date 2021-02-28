#pragma once

#include <od/extras/RealFFT.h>
#include <deque>
#define MAXPARTS 512

namespace od
{

  // Convolution via Uniform Partitioned Overlap-Save
  class UPOLS
  {
  public:
    UPOLS();
    virtual ~UPOLS();

    void shareIR(UPOLS &other);
    void setIR(float *data, int n, int stride = 1);
    void clearIR();
    void process(float *in, float *out);

  protected:
    RealFFT fft;

    RealBuffer mInput, mOutput;
    ComplexBuffer mSumBuffer;

    std::vector<complex_float_t *> mPartitions;
    std::deque<complex_float_t *> mFDL;
    ComplexBuffer mFDLBuffer;
    ComplexBuffer mPartitionBuffer;

    int P = 0; // number of partitions
    bool mReady = false;
    bool mShared = false;
  };

} /* namespace od */
