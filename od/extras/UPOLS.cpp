#include <od/extras/UPOLS.h>
#include <hal/simd.h>
#include <od/config.h>
#include <string.h>

namespace od
{

#if 0

static inline void complexMultiplyAndSet(complex_float_t * x,
		complex_float_t * y, complex_float_t * sum, int n) {
	for (int i = 0; i < n; i++) {
		float re = x[i].r * y[i].r - x[i].i * y[i].i;
		float im = x[i].r * y[i].i + x[i].i * y[i].r;
		sum[i].r = re;
		sum[i].i = im;
	}
}

static inline void complexMultiplyAndAccumulate(complex_float_t * x,
		complex_float_t * y, complex_float_t * sum, int n) {
	for (int i = 0; i < n; i++) {
		float re = x[i].r * y[i].r - x[i].i * y[i].i;
		float im = x[i].r * y[i].i + x[i].i * y[i].r;
		sum[i].r += re;
		sum[i].i += im;
	}
}

#else

  // NEON-optimized versions

  static inline void complexMultiplyAndAccumulate(complex_float_t *x,
                                                  complex_float_t *y, complex_float_t *sum, int n)
  {
    int i;
    int N = 4 * (n / 4);

    for (i = 0; i < N; i += 4)
    {
      float32x4x2_t X = vld2q_f32((float *)&x[i]);
      float32x4x2_t Y = vld2q_f32((float *)&y[i]);
      float32x4x2_t S = vld2q_f32((float *)&sum[i]);
      S.val[0] += X.val[0] * Y.val[0] - X.val[1] * Y.val[1];
      S.val[1] += X.val[0] * Y.val[1] + X.val[1] * Y.val[0];
      vst2q_f32((float *)&sum[i], S);
    }

    for (; i < n; i++)
    {
      float re = x[i].r * y[i].r - x[i].i * y[i].i;
      float im = x[i].r * y[i].i + x[i].i * y[i].r;
      sum[i].r += re;
      sum[i].i += im;
    }
  }

  static inline void complexMultiplyAndSet(complex_float_t *x,
                                           complex_float_t *y, complex_float_t *sum, int n)
  {
    int i;
    int N = 4 * (n / 4);

    for (i = 0; i < N; i += 4)
    {
      float32x4x2_t X = vld2q_f32((float *)&x[i]);
      float32x4x2_t Y = vld2q_f32((float *)&y[i]);
      float32x4x2_t S;
      S.val[0] = X.val[0] * Y.val[0] - X.val[1] * Y.val[1];
      S.val[1] = X.val[0] * Y.val[1] + X.val[1] * Y.val[0];
      vst2q_f32((float *)&sum[i], S);
    }

    for (; i < n; i++)
    {
      float re = x[i].r * y[i].r - x[i].i * y[i].i;
      float im = x[i].r * y[i].i + x[i].i * y[i].r;
      sum[i].r = re;
      sum[i].i = im;
    }
  }

#endif

  UPOLS::UPOLS()
  {
    int B = FRAMELENGTH;
    int BB = 2 * B;
    int B1 = B + 1;
    fft.allocate(BB);
    mInput.resize(BB, 0);
    mOutput.resize(BB, 0);

    complex_float_t zero;
    zero.r = 0;
    zero.i = 0;
    mSumBuffer.resize(B1, zero);
  }

  UPOLS::~UPOLS()
  {
  }

  void UPOLS::setIR(float *data, int n, int stride)
  {
    int B = FRAMELENGTH;
    int BB = 2 * B;
    int B1 = B + 1;
    int aB1 = CACHE_ALIGNED_SIZE(B1 * sizeof(complex_float_t)) / sizeof(complex_float_t);
    complex_float_t zero;
    zero.r = 0;
    zero.i = 0;

    clearIR();

    // Calculate number of partitions
    P = n / B;
    if (n % B != 0)
    {
      P++;
    }
    if (P > MAXPARTS)
    {
      P = MAXPARTS;
    }

    // Partition the IR and take FFT of each partition.
    // Allocate the FDL (freq delay line) while we are at it.
    if (P > 0)
    {
      int p, i, j = 0;
      RealBuffer tmp;
      tmp.resize(BB, 0);
      mPartitions.resize(P);
      mPartitionBuffer.resize(P * aB1, zero);
      mFDLBuffer.resize(P * aB1, zero);
      for (p = 0; p < P; p++)
      {
        for (i = 0; i < B && j < n; i++, j++)
        {
          tmp[i] = data[j * stride];
        }
        mPartitions[p] = &mPartitionBuffer[p * aB1];
        fft.compute(mPartitions[p], tmp.data());
        mFDL.push_back(&mFDLBuffer[p * aB1]);
      }

      mReady = true;
    }
  }

  void UPOLS::shareIR(UPOLS &other)
  {
    int B = FRAMELENGTH;
    int B1 = B + 1;
    int aB1 = CACHE_ALIGNED_SIZE(B1 * sizeof(complex_float_t)) / sizeof(complex_float_t);
    complex_float_t zero;
    zero.r = 0;
    zero.i = 0;

    clearIR();

    P = other.P;

    if (P > 0)
    {
      int p;
      mPartitions.resize(P);
      mFDLBuffer.resize(P * aB1, zero);
      for (p = 0; p < P; p++)
      {
        mPartitions[p] = &other.mPartitionBuffer[p * aB1];
        mFDL.push_back(&mFDLBuffer[p * aB1]);
      }

      mReady = true;
      mShared = true;
    }
  }

  void UPOLS::clearIR()
  {
    mReady = false;
    mShared = false;
    P = 0;
    mFDL.clear();
    mFDLBuffer.clear();
    mPartitionBuffer.clear();
    mPartitions.clear();
  }

  void UPOLS::process(float *in, float *out)
  {
    int B = FRAMELENGTH;

    if (!mReady)
    {
      // no impulse response
      memcpy(out, in, B * sizeof(float));
      return;
    }

    int B1 = B + 1;

    memcpy(mInput.data(), mInput.data() + B, B * sizeof(float));
    memcpy(mInput.data() + B, in, B * sizeof(float));

    complex_float_t *cb = mFDL.back();
    fft.compute(cb, mInput.data());
    mFDL.pop_back();
    mFDL.push_front(cb);

    complexMultiplyAndSet(mPartitions[0], mFDL[0], mSumBuffer.data(), B1);
    for (int p = 1; p < P; p++)
    {
      complexMultiplyAndAccumulate(mPartitions[p], mFDL[p], mSumBuffer.data(),
                                   B1);
    }

    fft.computeInverse(mOutput.data(), mSumBuffer.data());

    memcpy(out, mOutput.data() + B, B * sizeof(float));
  }

} /* namespace od */
