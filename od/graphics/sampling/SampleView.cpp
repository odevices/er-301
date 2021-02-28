#include <od/graphics/sampling/SampleView.h>
#include <od/config.h>
#include <hal/simd.h>
#include <limits>
#include <stdio.h>
#include <string.h>
#include <math.h>

#define MIN_VIEW_GAIN_DB -36
#define MAX_VIEW_GAIN_DB 36

namespace od
{

  static void floatToString(float input, const char *suffix,
                            std::string &result)
  {
    char tmp[16];
    float size = fabs(input);
    if (size < 1e-6)
    {
      snprintf(tmp, sizeof(tmp), "0%s", suffix);
    }
    else if (size < 1.0f)
    {
      snprintf(tmp, sizeof(tmp), "%1.3f%s", input, suffix);
    }
    else if (size < 10.0f)
    {
      snprintf(tmp, sizeof(tmp), "%2.2f%s", input, suffix);
    }
    else if (size < 100.0f)
    {
      snprintf(tmp, sizeof(tmp), "%3.1f%s", input, suffix);
    }
    else
    {
      snprintf(tmp, sizeof(tmp), "%4.0f%s", input, suffix);
    }
    result = tmp;
  }

  // assumes non-negative input
  static void secsToString(float input, std::string &result)
  {
    if (input < 1.0f)
    {
      floatToString(input * 1000, "ms", result);
    }
    else
    {
      floatToString(input, "s", result);
    }
  }

  static void decibelToString(int dB, std::string &result)
  {
    if (dB < 0)
    {
      char tmp[16];
      snprintf(tmp, sizeof(tmp), "%ddB", dB);
      result = tmp;
    }
    else if (dB > 0)
    {
      char tmp[16];
      snprintf(tmp, sizeof(tmp), "+%ddB", dB);
      result = tmp;
    }
    else
    {
      result = "0dB";
    }
  }

  SampleView::SampleView(int left, int bottom, int width, int height) : Graphic(left, bottom, width, height)
  {
    prepareVectors();
    own(mTimeZoomGadget);
    addChild(&mTimeZoomGadget);
    mTimeZoomGadget.setCenter(mWidth - 20, mHeight / 2);
    mTimeZoomGadget.setOpaque(true);
    mTimeZoomGadget.setCornerRadius(3, 3, 3, 3);
    mTimeZoomGadget.setBorder(1);
    mTimeZoomGadget.setJustification(justifyRight);
    own(mGainZoomGadget);
    addChild(&mGainZoomGadget);
    mGainZoomGadget.setCenter(mWidth - 20, mHeight / 2);
    mGainZoomGadget.setOpaque(true);
    mGainZoomGadget.setCornerRadius(3, 3, 3, 3);
    mGainZoomGadget.setBorder(1);
    mGainZoomGadget.setJustification(justifyRight);

    setViewTimeInSeconds(10.0f);
  }

  SampleView::~SampleView()
  {
    if (mpSample)
    {
      mpSample->release();
    }
  }

  void SampleView::prepareVectors()
  {
    mMaximums.resize(mWidth);
    mMinimums.resize(mWidth);
    for (int i = 0; i < mWidth; i++)
    {
      mMaximums[i] = 0;
      mMinimums[i] = 0;
    }
    setZoomLevel(mZoomLevel);
    mInvalidatedIntervals.clear();
  }

  void SampleView::setSize(int width, int height)
  {
    Graphic::setSize(width, height);
    prepareVectors();
    mZoomLevelSaved = 0; // force a refresh
  }

  bool SampleView::setSample(Sample *sample)
  {
    if (sample != mpSample)
    {
      float viewTime = getViewTimeInSeconds();

      if (mpSample)
      {
        mpSample->release();
      }

      mpSample = sample;
      if (mpSample)
      {
        mpSample->attach();
        mZoomLevelSaved = 0; // force a refresh
        mLastLoadCount = mpSample->mSampleLoadCount;
        setViewTimeInSeconds(viewTime);
        setLeftPosition(mStart);
      }

      mChannel = 0;
      return true;
    }
    return false;
  }

  void SampleView::setChannel(int channel)
  {
    int saved = mChannel;
    if (mpSample)
    {
      mChannel = CLAMP(0, (int)mpSample->mChannelCount, channel);
    }
    else
    {
      mChannel = 0;
    }
    if (saved != mChannel)
    {
      // force a complete refresh
      mZoomLevelSaved = 0;
    }
  }

  void SampleView::setZoomLevel(int level)
  {
    if (mpSample)
    {
      int maxZoomLevel = mpSample->mSampleCount / (4 * mWidth);
      mZoomLevel = MAX(1, MIN(level, maxZoomLevel));
    }
    else
    {
      mZoomLevel = level;
    }
    mSamplesPerPixel = mZoomLevel * 4;
    mPixelsPerSample = 1.0f / (float)mSamplesPerPixel;
  }

  void SampleView::setViewTimeInSeconds(float secs)
  {
    float rate;
    if (mpSample)
    {
      rate = mpSample->mSampleRate;
    }
    else
    {
      rate = globalConfig.sampleRate;
    }
    int n = secs * rate;
    setZoomLevel(n / (4 * mWidth));
  }

  float SampleView::getViewTimeInSeconds()
  {
    float period;
    if (mpSample)
    {
      period = mpSample->mSamplePeriod;
    }
    else
    {
      period = globalConfig.samplePeriod;
    }
    return (mZoomLevel * 4 * mWidth) * period;
  }

  void SampleView::setCenterPosition(int center)
  {
    if (mpSample == 0)
      return;

    int halfWidth = mZoomLevel * 2 * mWidth;
    if (center < halfWidth)
    {
      center = halfWidth;
    }

    if (center + halfWidth >= (int)mpSample->mSampleCount)
    {
      center = mpSample->mSampleCount - halfWidth - 1;
    }

    if (center < halfWidth)
    {
      refresh(0);
    }
    else
    {
      refresh(center - halfWidth);
    }
  }

  void SampleView::setLeftPosition(int left)
  {
    if (mpSample == 0)
      return;

    int width = (mZoomLevel * 4 * mWidth);

    if (left + width >= (int)mpSample->mSampleCount)
    {
      left = mpSample->mSampleCount - width - 1;
    }

    refresh(left);
  }

  void SampleView::setRightPosition(int right)
  {
    if (mpSample == 0)
      return;

    int width = (mZoomLevel * 4 * mWidth);

    if (right < width)
    {
      right = width;
    }

    refresh(right - width);
  }

  void SampleView::partialRefresh(int start, int sampleCount)
  {
    if (mpSample == 0)
      return;

    int b = 4 * mZoomLevel;
    int end = start + sampleCount;

    start = b * (start / b);
    end = b * (end / b) + b;

    // intersect with current view
    start = MAX(start, mStart);
    end = MIN(end, mEnd);

    if (start >= end)
    {
      return;
    }

    int i;
    int i0 = (start - mStart) / b;
    int i1 = (end - mStart) / b;

    if (end > (int)mpSample->mSampleCount)
    {
      int save = i1;
      i1 -= (end - mpSample->mSampleCount) / b + 1;
      for (i = i1; i < save; i++)
      {
        mMaximums[i] = 0;
        mMinimums[i] = 0;
      }
    }

    if (mpSample->mChannelCount == 1)
    {
      // determine # of samples per display column
      float *samples = mpSample->mpData + start;
      float dy = 0.5f * mHeight * mGain;

      for (i = i0; i < i1; i++)
      {
        float32x4_t x;
        float32x4_t max = vdupq_n_f32(-20.0f);
        float32x4_t min = vdupq_n_f32(20.0f);
        float *end = samples + b;

        while (samples < end)
        {
          x = vld1q_f32(samples);
          max = vmaxq_f32(max, x);
          min = vminq_f32(min, x);
          samples += 4;
        }

        float tmp[4];

        vst1q_f32(tmp, max);
        mMaximums[i] = (int)(dy * MAX(MAX(tmp[0], tmp[1]), MAX(tmp[2], tmp[3])));

        vst1q_f32(tmp, min);
        mMinimums[i] = (int)(dy * MIN(MIN(tmp[0], tmp[1]), MIN(tmp[2], tmp[3])));
      }
    }
    else if (mpSample->mChannelCount == 2)
    {
      float *samples = mpSample->mpData + start * 2 + mChannel;
      float dy = 0.5f * mHeight * mGain;

      for (i = i0; i < i1; i++)
      {
        float32x4x2_t x;
        float32x4_t max = vdupq_n_f32(-20.0f);
        float32x4_t min = vdupq_n_f32(20.0f);
        float *end = samples + 2 * b;

        while (samples < end)
        {
          x = vld2q_f32(samples);
          max = vmaxq_f32(max, x.val[0]);
          min = vminq_f32(min, x.val[0]);
          samples += 8;
        }

        float tmp[4];

        vst1q_f32(tmp, max);
        mMaximums[i] = (int)(dy * MAX(MAX(tmp[0], tmp[1]), MAX(tmp[2], tmp[3])));

        vst1q_f32(tmp, min);
        mMinimums[i] = (int)(dy * MIN(MIN(tmp[0], tmp[1]), MIN(tmp[2], tmp[3])));
      }
    }
  }

  void SampleView::refresh(int start)
  {
    if (mpSample == 0)
      return;

    int i;

    mStart = MIN(start, (int)mpSample->mSampleCount - 1);
    // move the start to the previous block boundary
    int b = 4 * mZoomLevel;
    mStart = b * (mStart / b);
    mEnd = mStart + b * mWidth;

    // for the zoom gadget
    mViewSizeInSeconds = b * mWidth * mpSample->mSamplePeriod;

    if (mZoomLevel == mZoomLevelSaved)
    {
      int from, to;
      bool done = false;

      if (mStart == mStartSaved)
      {
        done = true;
      }
      else
      {
        // check for overlaps
        int endSaved = mStartSaved + b * mWidth;
        if (mStart > mStartSaved && mStart < endSaved)
        {
          // moved forward
          int diff = mStart - mStartSaved;
          int bdiff = diff / b;
          memmove(mMaximums.data(), mMaximums.data() + bdiff,
                  (mWidth - bdiff) * sizeof(int));
          memmove(mMinimums.data(), mMinimums.data() + bdiff,
                  (mWidth - bdiff) * sizeof(int));

          partialRefresh(mEnd - diff, diff);
          mStartSaved = mStart;
          done = true;
        }
        else if (mEnd > mStartSaved && mEnd < endSaved)
        {
          // moved backward
          int diff = endSaved - mEnd;
          int bdiff = diff / b;
          memmove(mMaximums.data() + bdiff, mMaximums.data(),
                  (mWidth - bdiff) * sizeof(int));
          memmove(mMinimums.data() + bdiff, mMinimums.data(),
                  (mWidth - bdiff) * sizeof(int));

          partialRefresh(mStart, diff);
          mStartSaved = mStart;
          done = true;
        }
      }

      // refresh invalidated areas
      for (Interval &interval : mInvalidatedIntervals)
      {
        from = interval.from;
        to = interval.to;
        if (from > b)
          from -= b;
        if (from < to)
        {
          partialRefresh(from, to - from);
        }
      }
      mInvalidatedIntervals.clear();

      // refresh freshly loaded areas
      from = mLastLoadCount;
      to = mpSample->mSampleLoadCount;
      mLastLoadCount = to;
      // delay the update by one block to avoid partially loaded blocks
      if (from > b)
        from -= b;
      if (from < to)
      {
        partialRefresh(from, to - from);
      }

      if (done)
      {
        // nothing else to do
        return;
      }
    }

    int N = mWidth;
    if (mEnd > (int)mpSample->mSampleCount)
    {
      N -= (mEnd - mpSample->mSampleCount) / b + 1;
      for (i = N; i < mWidth; i++)
      {
        mMaximums[i] = 0;
        mMinimums[i] = 0;
      }
      mEnd = mpSample->mSampleCount;
    }

    if (mpSample->mChannelCount == 1)
    {
      // determine # of samples per display column
      float *samples = mpSample->mpData + mStart;
      float dy = 0.5f * mHeight * mGain;

      for (i = 0; i < N; i++)
      {
        float32x4_t x;
        float32x4_t max = vdupq_n_f32(-20.0f);
        float32x4_t min = vdupq_n_f32(20.0f);
        float *end = samples + b;

        while (samples < end)
        {
          x = vld1q_f32(samples);
          max = vmaxq_f32(max, x);
          min = vminq_f32(min, x);
          samples += 4;
        }

        float tmp[4];

        vst1q_f32(tmp, max);
        mMaximums[i] = (int)(dy * MAX(MAX(tmp[0], tmp[1]), MAX(tmp[2], tmp[3])));

        vst1q_f32(tmp, min);
        mMinimums[i] = (int)(dy * MIN(MIN(tmp[0], tmp[1]), MIN(tmp[2], tmp[3])));
      }
    }
    else if (mpSample->mChannelCount == 2)
    {
      // determine # of samples per display column
      float *samples = mpSample->mpData + mStart * 2 + mChannel;
      float dy = 0.5f * mHeight * mGain;

      for (i = 0; i < N; i++)
      {
        float32x4x2_t x;
        float32x4_t max = vdupq_n_f32(-20.0f);
        float32x4_t min = vdupq_n_f32(20.0f);
        float *end = samples + 2 * b;

        while (samples < end)
        {
          x = vld2q_f32(samples);
          max = vmaxq_f32(max, x.val[0]);
          min = vminq_f32(min, x.val[0]);
          samples += 8;
        }

        float tmp[4];

        vst1q_f32(tmp, max);
        mMaximums[i] = (int)(dy * MAX(MAX(tmp[0], tmp[1]), MAX(tmp[2], tmp[3])));

        vst1q_f32(tmp, min);
        mMinimums[i] = (int)(dy * MIN(MIN(tmp[0], tmp[1]), MIN(tmp[2], tmp[3])));
      }
    }

    mStartSaved = mStart;
    mZoomLevelSaved = mZoomLevel;
    mInvalidatedIntervals.clear();
  }

  void SampleView::draw(FrameBuffer &fb)
  {
    int i, x0, x1, y0, y1;
    int mid = mWorldBottom + mHeight / 2;

    x0 = mWorldLeft;
    x1 = x0 + mWidth;

    // draw grid lines
    y0 = mid;
    fb.line(GRAY3, x0, y0, x1, y0);
    y0 = mWorldBottom + mHeight / 4;
    fb.line(GRAY3, x0, y0, x1, y0);
    y0 = mWorldBottom + (3 * mHeight) / 4;
    fb.line(GRAY3, x0, y0, x1, y0);

    if (mpSample == 0)
    {
      return;
    }

    if (mSavedWaterMark != mpSample->mWaterMark)
    {
      mSavedWaterMark = mpSample->mWaterMark;
      forceRefresh();
    }

    if (mMarkStart < mEnd && mMarkEnd > mStart)
    {
      int startSample = MAX(mMarkStart, mStart);
      int endSample = MIN(mMarkEnd, mEnd);
      int start = (startSample - mStart) * mPixelsPerSample;
      int end = (endSample - mStart) * mPixelsPerSample;
      end = MIN(mWidth, end);

      // draw waveform using minmax info
      for (i = 0; i < start; i++)
      {
        fb.vline(GRAY3, mWorldLeft + i, mid + mMinimums[i],
                 mid + mMaximums[i]);
      }
      for (; i < end; i++)
      {
        fb.vline(mMarkColor, mWorldLeft + i, mid + MIN(0, mMinimums[i]) - 1,
                 mid + MAX(0, mMaximums[i]) + 1);
      }
      for (; i < mWidth; i++)
      {
        fb.vline(GRAY3, mWorldLeft + i, mid + mMinimums[i],
                 mid + mMaximums[i]);
      }

      y0 = mid + mMinimums[0];
      for (i = 1; i < start; i++)
      {
        y1 = mid + mMinimums[i];
        fb.line(GRAY7, mWorldLeft + i - 1, y0, mWorldLeft + i, y1);
        y0 = y1;
      }
      for (; i < end; i++)
      {
        y1 = mid + mMinimums[i];
        //fb.line(WHITE, mWorldLeft + i - 1, y0, mWorldLeft + i, y1);
        y0 = y1;
      }
      for (; i < mWidth; i++)
      {
        y1 = mid + mMinimums[i];
        fb.line(GRAY7, mWorldLeft + i - 1, y0, mWorldLeft + i, y1);
        y0 = y1;
      }

      y0 = mid + mMaximums[0];
      for (i = 1; i < start; i++)
      {
        y1 = mid + mMaximums[i];
        fb.line(GRAY7, mWorldLeft + i - 1, y0, mWorldLeft + i, y1);
        y0 = y1;
      }
      for (; i < end; i++)
      {
        y1 = mid + mMaximums[i];
        //fb.line(WHITE, mWorldLeft + i - 1, y0, mWorldLeft + i, y1);
        y0 = y1;
      }
      for (; i < mWidth; i++)
      {
        y1 = mid + mMaximums[i];
        fb.line(GRAY7, mWorldLeft + i - 1, y0, mWorldLeft + i, y1);
        y0 = y1;
      }
    }
    else
    {

      // draw waveform using minmax info
      for (i = 0; i < mWidth; i++)
      {
        fb.vline(GRAY3, mWorldLeft + i,
                 mid + mMinimums[i],
                 mid + mMaximums[i]);
      }

      y0 = mid + mMinimums[0];
      for (i = 1; i < mWidth; i++)
      {
        y1 = mid + mMinimums[i];
        fb.line(GRAY7, mWorldLeft + i - 1, y0, mWorldLeft + i, y1);
        y0 = y1;
      }

      y0 = mid + mMaximums[0];
      for (i = 1; i < mWidth; i++)
      {
        y1 = mid + mMaximums[i];
        fb.line(GRAY7, mWorldLeft + i - 1, y0, mWorldLeft + i, y1);
        y0 = y1;
      }
    }
    // draw load pointer
    drawPosition(fb, mLastLoadCount, "L");

#if 0
    {
        char text[128];
        snprintf(text, sizeof(text), "s=%lu e=%lu z=%d L=%lu", mStart, mEnd,
                mZoomLevel, mLastLoadCount);
        fb.text(WHITE, mWorldLeft + 5, mWorldBottom + mHeight - 12, text, 10);
    }
#endif
  }

  void SampleView::drawPositionOverview(FrameBuffer &fb, int position)
  {
    if (mpSample == 0)
      return;

    int x0, x1, y0;

    // draw overview
    x0 = mWorldLeft + (int)((mWidth - 1) * (mStart / (float)(mpSample->mSampleCount - 1)));
    x1 = mWorldLeft + (int)((mWidth - 1) * (mEnd / (float)(mpSample->mSampleCount - 1)));
    y0 = mWorldBottom + mHeight - 1;
    fb.hline(GRAY7, x0, x1, y0);
    fb.hline(GRAY7, x0, x1, y0 - 1);

    x0 = mWorldLeft + (int)((mWidth - 1) * (position / (float)(mpSample->mSampleCount - 1)));
    fb.pixel(WHITE, x0, y0);
    fb.pixel(WHITE, x0, y0 - 1);
  }

  void SampleView::drawPosition(FrameBuffer &fb, int position, const char *name,
                                int width)
  {
    if (position > mEnd)
    {
      mCursorState.x = mWorldLeft + mWidth - CURSOR_HEIGHT;
      mCursorState.y = mWorldBottom + mHeight / 2;
      mCursorState.orientation = CursorOrientation::cursorRight;
    }
    else if (position < mStart)
    {
      mCursorState.x = mWorldLeft + CURSOR_HEIGHT;
      mCursorState.y = mWorldBottom + mHeight / 2;
      mCursorState.orientation = CursorOrientation::cursorLeft;
    }
    else
    {
      int x0 = mWorldLeft + (int)(mPixelsPerSample * (position - mStart));
      int y0 = mWorldBottom + mHeight - 2;
      int w1 = width / 2;
      int w2 = width - w1;
      fb.clear(x0 - w1 - 1, y0 - 10, x0 + w2 + 1, y0);
      fb.text(WHITE, x0 - w1, y0 - 8, name, 10);
      fb.vline(GRAY7, x0, mWorldBottom, y0 - 11);
      fb.box(GRAY7, x0 - w1 - 1, y0 - 10, x0 + w2 + 1, y0);
      mCursorState.x = x0;
      mCursorState.y = y0;
      mCursorState.orientation = CursorOrientation::cursorDown;
    }
  }

  void SampleView::drawMiniPosition(FrameBuffer &fb, int position, int height,
                                    int y)
  {
    if (position < mEnd && position > mStart)
    {
      int x0 = mWorldLeft + (int)(mPixelsPerSample * (position - mStart));
      int y0 = mWorldBottom + mHeight / 2 + y;
      fb.vline(WHITE, x0, y0 - height / 2, y0 + height / 2);
    }
  }

  void SampleView::drawTimeZoomGadget(FrameBuffer &fb)
  {
    if (mSavedViewSizeInSeconds != mViewSizeInSeconds)
    {
      std::string text;
      mSavedViewSizeInSeconds = mViewSizeInSeconds;
      secsToString(mViewSizeInSeconds, text);
      mTimeZoomGadget.setText(text);
      mTimeZoomGadget.fitToText(3);
    }
    mTimeZoomGadget.draw(fb);
    mCursorState.x = mWorldLeft + mWidth - 10 + CURSOR_HEIGHT;
    mCursorState.y = mWorldBottom + mHeight / 2;
    mCursorState.orientation = CursorOrientation::cursorLeft;
  }

  void SampleView::drawGainZoomGadget(FrameBuffer &fb)
  {
    if (mSavedViewGain != mGainDecibels)
    {
      std::string text;
      mSavedViewGain = mGainDecibels;
      decibelToString(mGainDecibels, text);
      mGainZoomGadget.setText(text);
      mGainZoomGadget.fitToText(3);
    }
    mGainZoomGadget.draw(fb);
    mCursorState.x = mWorldLeft + mWidth - 10 + CURSOR_HEIGHT;
    mCursorState.y = mWorldBottom + mHeight / 2;
    mCursorState.orientation = CursorOrientation::cursorLeft;
  }

  bool SampleView::encoderZoom(int change, bool shifted, int threshold)
  {
    switch (mZoomGadgetState)
    {
    case showTimeGadget:
      return encoderZoomTime(change, shifted, threshold);
    case showGainGadget:
      return encoderZoomGain(change, shifted, threshold);
    default:
      return false;
    }
  }

  bool SampleView::encoderZoomGain(int change, bool shifted, int threshold)
  {
    if (mpSample == 0)
      return false;
    mEncoderSum += change;
    if (mEncoderSum > threshold)
    {
      mEncoderSum = 0;
      setViewGainInDecibels(mGainDecibels + 2);
      return true;
    }
    else if (mEncoderSum < -threshold)
    {
      mEncoderSum = 0;
      setViewGainInDecibels(mGainDecibels - 2);
      return true;
    }
    return false;
  }

  bool SampleView::encoderZoomTime(int change, bool shifted, int threshold)
  {
    if (mpSample == 0)
      return false;
    mEncoderSum += change;
    if (mEncoderSum > threshold)
    {
      mEncoderSum = 0;
      if (shifted)
      {
        setZoomLevel(1);
      }
      else
      {
        setZoomLevel(mZoomLevel / 2);
      }
      return true;
    }
    else if (mEncoderSum < -threshold)
    {
      mEncoderSum = 0;
      if (shifted)
      {
        setZoomLevel(std::numeric_limits<int>::max());
      }
      else
      {
        setZoomLevel(mZoomLevel * 2);
      }

      return true;
    }
    return false;
  }

  void SampleView::notifyVisible()
  {
    if (mpSample && (int)mpSample->mSampleLoadCount != mLastLoadCount)
    {
      // force a complete refresh
      mZoomLevelSaved = 0;
    }
  }

  void SampleView::forceRefresh()
  {
    mZoomLevelSaved = 0;
  }

  void SampleView::invalidateInterval(int from, int to)
  {
    from = MAX(from, mStart);
    to = MIN(to, mEnd);
    if (from < to)
    {
      mInvalidatedIntervals.emplace_back(from, to);
    }
  }

  void SampleView::setViewGainInDecibels(int decibels)
  {
    mGainDecibels = MAX(MIN_VIEW_GAIN_DB, MIN(MAX_VIEW_GAIN_DB, decibels));
    mGain = powf(10.0f, mGainDecibels / 20.0f);
    mZoomLevelSaved = 0;
  }

} /* namespace od */
