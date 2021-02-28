#include <od/audio/OnsetDetector.h>
#include <od/extras/RealFFT.h>
#include <od/extras/EWMA.h>
#include <od/extras/BigHeap.h>
#include <hal/ops.h>
#include <math.h>
#include <string.h>

#ifndef M_TWOPI
#define M_TWOPI (2*M_PI)
#endif

namespace od
{

  static inline void computeSquaredSpectrum(float *out,
                                            complex_float_t *fft, int n)
  {
    // skip DC coefficient
    fft++;

    for (int i = 0; i < n; i++)
    {
      out[i] = fft[i].r * fft[i].r + fft[i].i * fft[i].i;
    }
  }

  static inline void computeSpectrum(float *out, complex_float_t *fft,
                                     int n)
  {
    // skip DC coefficient
    fft++;

    for (int i = 0; i < n; i++)
    {
      //out[i] = sqrtf(fft[i].r * fft[i].r + fft[i].i * fft[i].i);
      out[i] = fabs(fft[i].r) + fabs(fft[i].i);
    }
  }

  static inline void computeWeightedSpectrum(float *out,
                                             complex_float_t *fft, int n)
  {
    // skip DC coefficient
    fft++;

    float scale = 1.0f / n;
    for (int i = 0; i < n; i++)
    {
      //out[i] = scale * i * sqrtf(fft[i].r * fft[i].r + fft[i].i * fft[i].i);
      out[i] = scale * i * (fabs(fft[i].r) + fabs(fft[i].i));
    }
  }

  static inline float computeRectifiedSpectralDifference(float *cur,
                                                         float *prev, int n)
  {
    float sum = 0.0f;
    for (int i = 0; i < n; i++)
    {
      float diff = cur[i] - prev[i];
      sum += diff + fabs(diff);
    }

    return sum;
  }

  static inline float computeModifiedKullbackLeibler(float *cur, float *prev,
                                                     int n)
  {
    float sum = 0.0f;
    for (int i = 0; i < n; i++)
    {
      sum += log(1 + cur[i] / (prev[i] + 0.01f));
    }

    return sum;
  }

  static inline float computeMean(float *cur, int n)
  {
    float sum = 0.0f;
    for (int i = 0; i < n; i++)
    {
      sum += cur[i];
    }

    return sum / n;
  }

  static inline float computeEnergy(float *cur, int n)
  {
    float sum = 0.0f;
    float mean = computeMean(cur, n);
    for (int i = 0; i < n; i++)
    {
      sum += fabs(cur[i] - mean);
    }

    return sum;
  }

  OnsetDetector::OnsetDetector()
  {
  }

  OnsetDetector::~OnsetDetector()
  {
    freeResources();
  }

  void OnsetDetector::freeResources()
  {
    if (mpBuffer)
    {
      BigHeap::free(mpBuffer);
      mpBuffer = 0;
      mLength = 0;
    }
  }

  bool OnsetDetector::computeSpectralFlux(Sample *sample, uint32_t start,
                                          uint32_t end, float targetRate,
                                          float overlap)
  {
    if (mpBuffer)
    {
      BigHeap::free(mpBuffer);
      mpBuffer = 0;
      mLength = 0;
    }

    // at least 1Hz
    targetRate = MAX(1, targetRate);
    // clip to [0.1,0.9]
    overlap = MIN(MAX(0.1f, overlap), 0.9f);

    // Determine STFT parameters
    int hopSize;
    int windowSize = 1;
    int targetWindowSize = sample->mSampleRate / targetRate;

    while (windowSize < targetWindowSize)
    {
      windowSize *= 2;
    }

    hopSize = MAX(1, targetWindowSize * (1 - overlap));
    mSampleRate = sample->mSampleRate / hopSize;

    int sampleCount = end - start;

    mLength = (sampleCount - windowSize) / hopSize;
    if (mLength < 10)
    {
      mLength = 0;
      return false;
    }
    mpBuffer = BigHeap::allocate(mLength * sizeof(float));
    if (mpBuffer == 0)
    {
      return false;
    }

    mOriginalSampleRate = sample->mSampleRate;
    mStartSample = start;

    int W = 4 * (windowSize / 4); // neon optimization
    int halfW = W / 2;
    std::vector<float> hamming(W);

    // Hamming Window
    for (int i = 0; i < W; i++)
    {
      hamming[i] = 0.54f - 0.46f * cosf(((float)M_TWOPI) * i / (float)(W - 1));
    }

    // Calculate spectral flux
    RealFFT fft(W);
    std::vector<float> spectrum(halfW), spectrum0(halfW, 0);
    RealBuffer fftIn(W);
    ComplexBuffer fftOut(halfW + 1);
    float *D = (float *)(mpBuffer);
    float sum = 0;
    float sum2 = 0;
    for (int i = 0, j = start; i < mLength; i++, j += hopSize)
    {
      if (sample->mChannelCount == 1)
      {
        for (int k = 0; k < W; k++)
        {
          fftIn[k] = hamming[k] * sample->mpData[j + k];
        }
      }
      else if (sample->mChannelCount == 2)
      {
        for (int k = 0; k < W; k++)
        {
          fftIn[k] = 0.5f * hamming[k] * (sample->mpData[2 * (j + k)] + sample->mpData[2 * (j + k) + 1]);
        }
      }

      fft.compute(fftOut.data(), fftIn.data());
      computeSpectrum(spectrum.data(), fftOut.data(), halfW);
      D[i] = computeRectifiedSpectralDifference(spectrum.data(),
                                                spectrum0.data(), halfW);
      memcpy(spectrum0.data(), spectrum.data(), sizeof(float) * halfW);
      sum += D[i];
      sum2 += D[i] * D[i];
    }

    // Normalize to zero-mean, unit-sigma
    float mean = sum / mLength;
    float sigma = sqrtf(sum2 / mLength - mean * mean);
    float normalize = 1.0f / sigma;
    for (int i = 0; i < mLength; i++)
    {
      D[i] = (D[i] - mean) * normalize;
    }

    return true;
  }

  /* Some sample C code for the quickselect algorithm,
 taken from Numerical Recipes in C. */

#define SWAP(a, b) \
  temp = (a);      \
  (a) = (b);       \
  (b) = temp;

  static float quickselect(float *arr, int n, int k)
  {
    int i, ir, j, l, mid;
    float a, temp;

    l = 0;
    ir = n - 1;
    for (;;)
    {
      if (ir <= l + 1)
      {
        if (ir == l + 1 && arr[ir] < arr[l])
        {
          SWAP(arr[l], arr[ir]);
        }
        return arr[k];
      }
      else
      {
        mid = (l + ir) >> 1;
        SWAP(arr[mid], arr[l + 1]);
        if (arr[l] > arr[ir])
        {
          SWAP(arr[l], arr[ir]);
        }
        if (arr[l + 1] > arr[ir])
        {
          SWAP(arr[l + 1], arr[ir]);
        }
        if (arr[l] > arr[l + 1])
        {
          SWAP(arr[l], arr[l + 1]);
        }
        i = l + 1;
        j = ir;
        a = arr[l + 1];
        for (;;)
        {
          do
            i++;
          while (arr[i] < a);
          do
            j--;
          while (arr[j] > a);
          if (j < i)
            break;
          SWAP(arr[i], arr[j]);
        }
        arr[l + 1] = arr[j];
        arr[j] = a;
        if (j >= k)
          ir = j - 1;
        if (j <= k)
          l = i;
      }
    }
  }

  int OnsetDetector::findPeaks(float peakThreshold, float analysisWindow,
                               float coolDownPeriod, Slices *pSlices,
                               float offset)
  {
    if (mpBuffer == 0)
      return 0;

    // max number of slices corresponds to 20 slices per second
    int maxCount = mLength * mSampleRate * 20;
    int halfWidth = MAX(3, 0.5f * analysisWindow * mSampleRate);
    int coolDown = MAX(halfWidth, coolDownPeriod * mSampleRate);
    int width = 2 * halfWidth + 1;
    float conversionFactor = mOriginalSampleRate / mSampleRate;
    int offsetInSamples = (int)(offset * mOriginalSampleRate);
    float *D = (float *)(mpBuffer);
    std::vector<float> tmp(width, 0);

    int n = 0;
    int sinceLast = coolDown;
    float clamp = 1000.0f;
    pSlices->clear();
    for (int i = 0; i < mLength - halfWidth; i++)
    {
      // cool down
      if (sinceLast < coolDown)
      {
        // signal must fall below clamp
        if (D[i] < clamp)
        {
          sinceLast++;
        }
        continue;
      }

      clamp = 1000.0f;

      if (D[i] > peakThreshold)
      {
        memcpy(tmp.data(), D + MAX(0, i - halfWidth), width * sizeof(float));
        float median = quickselect(tmp.data(), width, halfWidth);
        if (D[i] - median > peakThreshold)
        {
          clamp = median + peakThreshold;
          if (pSlices)
          {
            uint32_t pos = (uint32_t)(i * conversionFactor);
            pos += mStartSample;
            pos += offsetInSamples;
            pSlices->lock();
            pSlices->insert(pos);
            pSlices->unlock();
          }
          sinceLast = 0;
          n++;

          if (n >= maxCount)
          {
            return n;
          }
        }
      }
    }

    return n;
  }

} /* namespace od */
