#include <od/graphics/sampling/SlicingViewMainDisplay.h>
#include <limits>
#include <math.h>
#include <stdio.h>

namespace od
{

  SlicingViewMainDisplay::SlicingViewMainDisplay(SliceHead *head) : SliceHeadDisplay(head, 0, 0, 256, 64)
  {
    own(mOnsetGadget);
    addChild(&mOnsetGadget);
    mOnsetGadget.setCenter(mWidth - 20, mHeight / 2);
    mOnsetGadget.setOpaque(true);
    mOnsetGadget.setCornerRadius(3, 3, 3, 3);
    mOnsetGadget.setBorder(1);
    mOnsetGadget.setJustification(justifyRight);

    own(mGridGadget);
    addChild(&mGridGadget);
    mGridGadget.setCenter(mWidth - 20, mHeight / 2);
    mGridGadget.setOpaque(true);
    mGridGadget.setCornerRadius(3, 3, 3, 3);
    mGridGadget.setBorder(1);
    mGridGadget.setJustification(justifyRight);

    own(mShiftGadget);
    addChild(&mShiftGadget);
    mShiftGadget.setCenter(mWidth - 20, mHeight / 2);
    mShiftGadget.setOpaque(true);
    mShiftGadget.setCornerRadius(3, 3, 3, 3);
    mShiftGadget.setBorder(1);
    mShiftGadget.setJustification(justifyRight);
  }

  SlicingViewMainDisplay::~SlicingViewMainDisplay()
  {
  }

  void SlicingViewMainDisplay::updateMarking()
  {
    if (mMarkState == 1)
    {
      mMarkState = 2;
      mAnchor = mPendingAnchor;
    }
    if (mMarkState == 2)
    {
      if (mPointer > mAnchor)
      {
        mSampleView.setMarkedRegion(mAnchor, mPointer);
      }
      else
      {
        mSampleView.setMarkedRegion(mPointer, mAnchor);
      }
    }
  }

  bool SlicingViewMainDisplay::encoderPointer(int change, bool shifted,
                                              float sensitivity)
  {
    change = mEncoderHysteresis.onChange(change);
    movePointerByProportion(change * sensitivity);
    return true;
  }

  static void thresholdToString(float threshold, int n, std::string &result)
  {
    char tmp[64];
    if (threshold < 1e-6)
    {
      snprintf(tmp, sizeof(tmp), "0: %d slices", n);
    }
    else if (threshold < 1.0f)
    {
      snprintf(tmp, sizeof(tmp), "%1.3f: %d slices", threshold, n);
    }
    else if (threshold < 10.0f)
    {
      snprintf(tmp, sizeof(tmp), "%2.2f: %d slices", threshold, n);
    }
    else if (threshold < 100.0f)
    {
      snprintf(tmp, sizeof(tmp), "%3.1f: %d slices", threshold, n);
    }
    else
    {
      snprintf(tmp, sizeof(tmp), "%4.0f: %d slices", threshold, n);
    }
    result = tmp;
  }

  bool SlicingViewMainDisplay::encoderOnsetThreshold(int change, bool shifted,
                                                     float sensitivity)
  {
    change = mEncoderHysteresis.onChange(change);
    mOnsetThreshold = MAX(0.0f, mOnsetThreshold + change * sensitivity);
    mGadgetSlices.clear();
    int n = slicer.findPeaks(mOnsetThreshold, 0.1f, 0.05f, &mGadgetSlices,
                             0.001f);

    std::string text;
    thresholdToString(mOnsetThreshold, n, text);
    mOnsetGadget.setText(text);
    mOnsetGadget.fitToText(3);
    return true;
  }

  static void periodToString(double input, std::string &result)
  {
    char tmp[16];
    float size = fabs(input);
    if (size < 1e-6)
    {
      snprintf(tmp, sizeof(tmp), "0s");
    }
    else if (size < 1.0f)
    {
      snprintf(tmp, sizeof(tmp), "%1.3fs", input);
    }
    else if (size < 10.0f)
    {
      snprintf(tmp, sizeof(tmp), "%2.2fs", input);
    }
    else if (size < 100.0f)
    {
      snprintf(tmp, sizeof(tmp), "%3.1fs", input);
    }
    else
    {
      snprintf(tmp, sizeof(tmp), "%4.0fs", input);
    }
    result = tmp;
  }

  bool SlicingViewMainDisplay::encoderGridPeriod(int change, bool shifted,
                                                 int sensitivity)
  {
    change = mEncoderHysteresis.onChange(change);
    mGridPeriodInMS = MAX(50, mGridPeriodInMS + change * sensitivity);
    mGridPeriod = 0.001 * mGridPeriodInMS;
    std::string text;
    periodToString(mGridPeriod, text);
    mGridGadget.setText(text);
    mGridGadget.fitToText(3);
    return true;
  }

  static void divisionToString(int division, double period, std::string &result)
  {
    char tmp[32];
    float size = fabs(period);
    if (size < 1e-6)
    {
      snprintf(tmp, sizeof(tmp), "%d (0s)", division);
    }
    else if (size < 1.0f)
    {
      snprintf(tmp, sizeof(tmp), "%d (%1.3fs)", division, period);
    }
    else if (size < 10.0f)
    {
      snprintf(tmp, sizeof(tmp), "%d (%2.2fs)", division, period);
    }
    else if (size < 100.0f)
    {
      snprintf(tmp, sizeof(tmp), "%d (%3.1fs)", division, period);
    }
    else
    {
      snprintf(tmp, sizeof(tmp), "%d (%4.0fs)", division, period);
    }
    result = tmp;
  }

  void SlicingViewMainDisplay::updateGridPeriodFromGridDivision()
  {
    Sample *pSample = mpHead->getSample();

    if (pSample == 0)
      return;

    // entire grid
    int start = MIN(mSampleView.mMarkStart, (int)pSample->mSampleCount);
    int end = MIN(mSampleView.mMarkEnd, (int)pSample->mSampleCount);

    if (start >= end)
    {
      start = 0;
      end = pSample->mSampleCount;
    }

    if (end > start)
    {
      double duration = pSample->calculateDurationRobustly(end - start);
      mGridPeriod = duration / mGridDivision;
    }
    else
    {
      mGridPeriod = 0;
    }
  }

  bool SlicingViewMainDisplay::encoderGridDivision(int change, bool shifted,
                                                   int sensitivity)
  {
    change = mEncoderHysteresis.onChange(change);
    mGridDivision = CLAMP(1, 4096, mGridDivision + change * sensitivity);
    updateGridPeriodFromGridDivision();

    std::string text;
    divisionToString(mGridDivision, mGridPeriod, text);
    mGridGadget.setText(text);
    mGridGadget.fitToText(3);
    return true;
  }

  bool SlicingViewMainDisplay::encoderSliceShift(int change, bool shifted,
                                                 int sensitivity)
  {
    change = mEncoderHysteresis.onChange(change);
    mSliceShiftInMS = mSliceShiftInMS + change * sensitivity;
    mSliceShift = 0.001f * mSliceShiftInMS;
    std::string text;
    periodToString(mSliceShift, text);
    mShiftGadget.setText(text);
    mShiftGadget.fitToText(3);
    return true;
  }

  void SlicingViewMainDisplay::draw(FrameBuffer &fb)
  {
    int playPosition = 0;
    if (mpHead)
    {
      Sample *pSample = mpHead->getSample();
      if (mSampleView.setSample(pSample))
      {
        // Sample changed, notify
        mPointer = (mSampleView.mStart + mSampleView.mEnd) / 2;
        updateMarking();
      }
      playPosition = mpHead->getPosition();
    }

    if (mFollowSliceHead)
    {
      mPointer = playPosition;
    }

    mSampleView.setCenterPosition(mPointer);
    mSampleView.draw(fb);
    mSampleView.drawPositionOverview(fb, playPosition);
    drawSlices(fb);

    if (mActiveGadget == Gadget::none)
    {
      mSampleView.drawPosition(fb, playPosition, "P");

      // draw pointer
      if (mPointer > mSampleView.mEnd)
      {
        mCursorState.x = mWorldLeft + mWidth - CURSOR_HEIGHT;
        mCursorState.y = mWorldBottom + mHeight / 2;
        mCursorState.orientation = CursorOrientation::cursorRight;
      }
      else if (mPointer < mSampleView.mStart)
      {
        mCursorState.x = mWorldLeft + CURSOR_HEIGHT;
        mCursorState.y = mWorldBottom + mHeight / 2;
        mCursorState.orientation = CursorOrientation::cursorLeft;
      }
      else
      {
        // pointer is visible
        int x0 = mWorldLeft + (int)(mSampleView.mPixelsPerSample * (mPointer - mSampleView.mStart));
        int y0 = mWorldBottom + mHeight - 3;
        fb.vline(WHITE, x0, mWorldBottom, y0);
        fb.drawDownTriangle(WHITE, x0, y0, 4);
        mCursorState.x = x0;
        mCursorState.y = y0;
        mCursorState.orientation = CursorOrientation::cursorDown;
      }
    }

    switch (mActiveGadget)
    {
    case Gadget::onsetGadget:
      drawOnsetHelper(fb);
      mOnsetGadget.draw(fb);
      break;
    case Gadget::periodGadget:
    case Gadget::divisionGadget:
      drawGridHelper(fb);
      mGridGadget.draw(fb);
      break;
    case Gadget::shiftGadget:
      drawShiftHelper(fb);
      mShiftGadget.draw(fb);
      break;
    default:
      break;
    }

    switch (mZoomGadgetState)
    {
    case showTimeGadget:
      mSampleView.drawTimeZoomGadget(fb);
      mCursorState.copyAttributes(mSampleView.mCursorState);
      break;
    case showGainGadget:
      mSampleView.drawGainZoomGadget(fb);
      mCursorState.copyAttributes(mSampleView.mCursorState);
      break;
    case gadgetHidden:
      break;
    }

    drawStatus(fb);
  }

  void SlicingViewMainDisplay::drawStatus(FrameBuffer &fb)
  {
    const char *status = 0;
    switch (mZoomGadgetState)
    {
    case showTimeGadget:
      status = "zooming: time";
      break;
    case showGainGadget:
      status = "zooming: height";
      break;
    case gadgetHidden:
      switch (mActiveGadget)
      {
      case Gadget::onsetGadget:
        status = "onset";
        break;
      case Gadget::periodGadget:
        status = "grid";
        break;
      case Gadget::shiftGadget:
        status = "shift";
        break;
      default:
        if (isPaused())
        {
          status = "paused";
        }
        break;
      }
      break;
    }

    if (status)
    {
      fb.text(WHITE, mWorldLeft + 5, mWorldBottom + mHeight - 8, status, 10);
    }
  }

  void SlicingViewMainDisplay::drawOnsetHelper(FrameBuffer &fb)
  {
    int x0, y0;
    SlicesIterator i = mGadgetSlices.getPositionAfter(mSampleView.mStart);
    SlicesIterator end = mGadgetSlices.end();
    y0 = mWorldBottom + mHeight - 1;
    while (i < end)
    {
      Slice &slice = *i;
      if (slice.mStart > mSampleView.mEnd)
        break;
      x0 = mWorldLeft + (int)(mSampleView.mPixelsPerSample * (slice.mStart - mSampleView.mStart));
      fb.vline(GRAY10, x0, mWorldBottom, y0);
      i++;
    }
  }

  void SlicingViewMainDisplay::drawShiftHelper(FrameBuffer &fb)
  {
    if (mpHead == 0)
      return;

    Sample *pSample = mpHead->getSample();
    Slices *pSlices = slices();

    if (pSample == 0 || pSlices == 0)
      return;

    // entire grid
    int start = MIN(mSampleView.mMarkStart, (int)pSample->mSampleCount);
    int end = MIN(mSampleView.mMarkEnd, (int)pSample->mSampleCount);

    if (start >= end)
    {
      start = 0;
      end = pSample->mSampleCount;
    }
    else if (start > 0)
    {
      // let's take a little bit more for shifting
      start--;
    }

    int shift = mSliceShift * pSample->mSampleRate;

    int x0, y0;
    SlicesIterator i = pSlices->getPositionAfter(start);
    SlicesIterator n = pSlices->end();
    y0 = mWorldBottom + mHeight - 1;
    while (i < n)
    {
      Slice &slice = *i;
      if (slice.mStart > end)
        break;
      int pos = slice.mStart + shift;
      if (pos < 0)
      {
        pos += pSample->mSampleCount;
      }
      else if (pos > (int)pSample->mSampleCount)
      {
        pos -= pSample->mSampleCount;
      }
      x0 = mWorldLeft + (int)(mSampleView.mPixelsPerSample * (pos - mSampleView.mStart));
      if (x0 >= 0 && x0 <= mWidth)
      {
        fb.vline(GRAY10, x0, mWorldBottom, y0);
      }
      i++;
    }
  }

  void SlicingViewMainDisplay::drawGridHelper(FrameBuffer &fb)
  {
    if (mpHead == 0)
      return;

    Sample *pSample = mpHead->getSample();

    if (pSample == 0 || mGridPeriod < 0.001)
      return;

    // entire grid
    int start = MIN(mSampleView.mMarkStart, (int)pSample->mSampleCount);
    int end = MIN(mSampleView.mMarkEnd, (int)pSample->mSampleCount);

    if (start >= end)
    {
      start = 0;
      end = pSample->mSampleCount;
    }

    // visible portion of grid
    int start0 = MAX(start, mSampleView.mStart);
    int end0 = MIN(end, mSampleView.mEnd);

    // convert to seconds
    float samplePeriod = 1.0f / pSample->mSampleRate;
    float startTime = start * samplePeriod;
    float start0Time = start0 * samplePeriod;
    float end0Time = end0 * samplePeriod;
    float viewStartTime = mSampleView.mStart * samplePeriod;

    float x = startTime;
    while (x < start0Time)
    {
      x += mGridPeriod;
    }

    float pixelsPerSecond = pSample->mSampleRate * mSampleView.mPixelsPerSample;
    while (x < end0Time)
    {
      int i = (x - viewStartTime) * pixelsPerSecond;
      fb.vline(GRAY10, mWorldLeft + i, mWorldBottom, mWorldBottom + mHeight);
      x += mGridPeriod;
    }
  }

  void SlicingViewMainDisplay::insertSlice(bool snapToZeroCrossing)
  {
    if (mpHead)
    {
      Slices *pSlices = slices();
      if (pSlices)
      {
        if (snapToZeroCrossing)
        {
          int nearest = findNearestZeroCrossing(
              mpHead->mpSample, mPointer, mSampleView.mStart,
              mSampleView.mStart + mSampleView.mEnd);
          pSlices->lock();
          pSlices->insert(nearest);
          pSlices->unlock();
        }
        else
        {
          pSlices->lock();
          pSlices->insert(mPointer);
          pSlices->unlock();
        }
      }
    }
  }

  void SlicingViewMainDisplay::deleteSlice()
  {
    if (mpHead)
    {
      Slices *pSlices = slices();
      if (pSlices)
      {
        int pos = mPointer;
        // Adjust backwards by one pixel just in case the pointer is drawn on top of a slice.
        if (pos > mSampleView.mSamplesPerPixel)
        {
          pos -= mSampleView.mSamplesPerPixel;
        }
        pSlices->lock();
        SlicesIterator i = pSlices->getPositionAfter(pos);
        if (i != pSlices->end())
        {
          Slice &slice = *i;
          if (slice.mStart < mSampleView.mEnd + mSampleView.mSamplesPerPixel)
          {
            pSlices->removeIterator(i);
          }
        }
        pSlices->unlock();
      }
    }
  }

  void SlicingViewMainDisplay::clearSlices()
  {
    if (mpHead)
    {
      Slices *pSlices = slices();
      if (pSlices)
      {
        pSlices->lock();
        pSlices->clear();
        pSlices->unlock();
      }
    }
  }

  bool SlicingViewMainDisplay::haveSliceUnderPointer()
  {
    bool result = false;
    if (mpHead)
    {
      Slices *pSlices = slices();
      if (pSlices)
      {
        pSlices->lock();
        // Adjust forward by one pixel just in case the pointer is drawn on top of a slice.
        SlicesIterator i = pSlices->getPositionBefore(
            mPointer + mSampleView.mSamplesPerPixel);
        if (i != pSlices->end())
        {
          Slice &slice = *i;
          result = slice.mStart <= mPointer;
        }
        pSlices->unlock();
      }
    }
    return result;
  }

  void SlicingViewMainDisplay::setLoopStart(bool snapToZeroCrossing)
  {
    if (mpHead)
    {
      Slices *pSlices = slices();
      if (pSlices)
      {
        int pos;
        if (snapToZeroCrossing)
        {
          pos = findNearestZeroCrossing(
              mpHead->mpSample, mPointer, mSampleView.mStart,
              mSampleView.mStart + mSampleView.mEnd);
        }
        else
        {
          pos = mPointer;
        }

        pSlices->lock();
        // Adjust forward by one pixel just in case the pointer is drawn on top of a slice.
        SlicesIterator i = pSlices->getPositionBefore(
            mPointer + mSampleView.mSamplesPerPixel);
        if (i != pSlices->end())
        {
          Slice &slice = *i;
          slice.mLoopStart = MAX(slice.mStart, pos);
        }
        pSlices->unlock();
      }
    }
  }

  void SlicingViewMainDisplay::setLoopEnd(bool snapToZeroCrossing)
  {
    if (mpHead)
    {
      Slices *pSlices = slices();
      if (pSlices)
      {
        int pos;
        if (snapToZeroCrossing)
        {
          pos = findNearestZeroCrossing(
              mpHead->mpSample, mPointer, mSampleView.mStart,
              mSampleView.mStart + mSampleView.mEnd);
        }
        else
        {
          pos = mPointer;
        }

        pSlices->lock();
        // Adjust forward by one pixel just in case the pointer is drawn on top of a slice.
        SlicesIterator i = pSlices->getPositionBefore(
            mPointer + mSampleView.mSamplesPerPixel);
        if (i != pSlices->end())
        {
          Slice &slice = *i;
          slice.mLoopEnd = MAX(slice.mStart, pos);
        }
        pSlices->unlock();
      }
    }
  }

  int SlicingViewMainDisplay::findNearestZeroCrossing(Sample *pSample, int pos,
                                                      int windowStart,
                                                      int windowEnd)
  {
    if (pSample == 0)
    {
      return pos;
    }

    windowStart = MIN(windowStart, (int)pSample->mSampleCount);
    windowEnd = MIN(windowEnd, (int)pSample->mSampleCount);

    if (windowStart > pos || windowEnd < pos)
      return pos;

    int i, j0, j1, n, ch;

    ch = pSample->mChannelCount;

    bool found = false;
    float *data = pSample->mpData + pos * ch;
    // search forward
    n = windowEnd - pos;
    n--;
    n *= ch;
    for (i = 0, j0 = 0; i < n; i += ch, j0++)
    {
      if (data[i] * data[i + ch] <= 0)
      {
        found = true;
        break;
      }
    }

    // search backward
    n = pos - windowStart;
    n--;
    n *= ch;
    for (i = 0, j1 = 0; i < n; i += ch, j1++)
    {
      if (data[-i] * data[-i - ch] <= 0)
      {
        found = true;
        break;
      }
    }

    if (found)
    {
      if (j0 < j1)
      {
        pos += j0;
      }
      else
      {
        pos -= j1;
      }
    }

    return pos;
  }

  int SlicingViewMainDisplay::movePointerToNearestZeroCrossing()
  {
    if (mpHead == 0)
      return 0;

    Sample *pSample = mpHead->getSample();

    if (pSample == 0)
      return 0;

    int save = mPointer;
    mPointer = findNearestZeroCrossing(pSample, mPointer, mSampleView.mStart,
                                       mSampleView.mStart + mSampleView.mEnd);

    updateMarking();
    return (int)mPointer - save;
  }

  void SlicingViewMainDisplay::movePointerByProportion(float amount)
  {
    int w = mSampleView.mSamplesPerPixel * mWidth;
    movePointerInSamples((int)(w * amount));
  }

  void SlicingViewMainDisplay::movePointerInSamples(int amount)
  {
    if (mpHead == 0)
      return;

    Sample *pSample = mpHead->getSample();

    if (pSample == 0)
      return;

    if (amount > 0)
    {
      int amt = (int)amount;
      int remaining = pSample->mSampleCount - mPointer - 1;
      if (amt < remaining)
      {
        mPointer += amt;
      }
      else
      {
        mPointer = pSample->mSampleCount - 1;
      }
    }
    else if (amount < 0)
    {
      int amt = (int)(-amount);
      int remaining = mPointer;
      if (amt < remaining)
      {
        mPointer -= amt;
      }
      else
      {
        mPointer = 0;
      }
    }

    updateMarking();
  }

  void SlicingViewMainDisplay::setPointer(int position)
  {
    mPointer = position;
    updateMarking();
  }

  void SlicingViewMainDisplay::movePointerToHead()
  {
    if (mpHead)
    {
      setPointer(mpHead->getPosition());
    }
  }

  void SlicingViewMainDisplay::movePointerToViewCenter()
  {
    setPointer((mSampleView.mStart + mSampleView.mEnd) / 2);
  }

  void SlicingViewMainDisplay::insertOnsetSlices(bool snapToZeroCrossing)
  {
    if (mpHead == 0)
      return;

    Sample *pSample = mpHead->getSample();
    Slices *pSlices = slices();

    if (pSample == 0 || pSlices == 0)
      return;

    // entire grid
    int start = MIN(mSampleView.mMarkStart, (int)pSample->mSampleCount);
    int end = MIN(mSampleView.mMarkEnd, (int)pSample->mSampleCount);

    if (start >= end)
    {
      start = 0;
      end = pSample->mSampleCount;
    }

    pSlices->removeRangeOfValues(start, end);

    slicer.findPeaks(mOnsetThreshold, 0.1f, 0.05f, pSlices, 0.001f);
  }

  void SlicingViewMainDisplay::insertDivisionSlices(bool snapToZeroCrossing)
  {
    updateGridPeriodFromGridDivision();
    insertPeriodSlices(snapToZeroCrossing);
  }

  void SlicingViewMainDisplay::insertPeriodSlices(bool snapToZeroCrossing)
  {
    if (mpHead == 0)
      return;

    Sample *pSample = mpHead->getSample();
    Slices *pSlices = slices();

    if (pSample == 0 || pSlices == 0 || mGridPeriod < 0.001)
      return;

    // entire grid
    int start = MIN(mSampleView.mMarkStart, (int)pSample->mSampleCount);
    int end = MIN(mSampleView.mMarkEnd, (int)pSample->mSampleCount);

    if (start >= end)
    {
      start = 0;
      end = pSample->mSampleCount;
    }

    pSlices->removeRangeOfValues(start, end);

    // convert to seconds
    double samplePeriod = 1.0 / pSample->mSampleRate;
    double startTime = start * samplePeriod;
    double endTime = end * samplePeriod;
    int halfGrid = (int)(mGridPeriod * pSample->mSampleRate * 0.5f);

    double x = startTime;
    while (x < endTime)
    {
      int i = x * pSample->mSampleRate + 0.5f;
      if (snapToZeroCrossing)
      {
        int snap = findNearestZeroCrossing(pSample, i, i - halfGrid,
                                           i + halfGrid);
        if (snap < (int)pSample->mSampleCount)
        {
          pSlices->lock();
          pSlices->insert(snap);
          pSlices->unlock();
        }
      }
      else
      {
        if (i < (int)pSample->mSampleCount)
        {
          pSlices->lock();
          pSlices->insert(i);
          pSlices->unlock();
        }
      }
      x += mGridPeriod;
    }
  }

  void SlicingViewMainDisplay::shiftSlices(bool wrap)
  {
    if (mpHead == 0)
      return;

    Sample *pSample = mpHead->getSample();
    Slices *pSlices = slices();

    if (pSample == 0 || pSlices == 0)
      return;

    // entire grid
    int start = MIN(mSampleView.mMarkStart, (int)pSample->mSampleCount);
    int end = MIN(mSampleView.mMarkEnd, (int)pSample->mSampleCount);

    if (start >= end)
    {
      start = 0;
      end = pSample->mSampleCount;
    }
    else if (start > 0)
    {
      // let's take a little bit more for shifting
      start--;
    }

    int shift = mSliceShift * pSample->mSampleRate;

    if (wrap)
    {
      SlicesIterator i = pSlices->getPositionAfter(start);
      SlicesIterator n = pSlices->end();

      while (i < n)
      {
        Slice &slice = *i;
        if (slice.mStart > end)
          break;

        slice.mStart += shift;
        if (slice.mStart > (int)pSample->mSampleCount)
        {
          slice.mStart -= pSample->mSampleCount;
        }
        else if (slice.mStart < 0)
        {
          slice.mStart += pSample->mSampleCount;
        }

        i++;
      }

      pSlices->sort();
    }
    else
    {
      if (shift > 0)
      {
        SlicesIterator i = pSlices->getPositionAfter(start);
        SlicesIterator n = pSlices->end();

        int toDelete = 0;
        while (i < n)
        {
          Slice &slice = *i;
          if (slice.mStart > end)
            break;
          if (slice.mStart + shift < (int)pSample->mSampleCount)
          {
            slice.mStart += shift;
          }
          else
          {
            slice.mStart = (int)pSample->mSampleCount;
            toDelete++;
          }
          i++;
        }

        if (toDelete > 0)
        {
          pSlices->lock();
          pSlices->removeRangeOfIterators(pSlices->end() - toDelete,
                                          pSlices->end());
          pSlices->unlock();
        }
      }
      else if (shift < 0)
      {
        shift = -shift;
        SlicesIterator i = pSlices->getPositionAfter(start);
        SlicesIterator n = pSlices->end();

        int toDelete = 0;
        while (i < n)
        {
          Slice &slice = *i;
          if (slice.mStart > end)
            break;
          if (slice.mStart < (int)shift)
          {
            slice.mStart = 0;
            toDelete++;
          }
          else
          {
            slice.mStart -= shift;
          }
          i++;
        }

        if (toDelete > 0)
        {
          pSlices->lock();
          pSlices->removeRangeOfIterators(pSlices->begin(),
                                          pSlices->begin() + toDelete);
          pSlices->unlock();
        }
      }
    }
  }

  void SlicingViewMainDisplay::beginMarking()
  {
    mPendingAnchor = mPointer;
    mMarkState = 1;
  }

  void SlicingViewMainDisplay::endMarking()
  {
    mMarkState = 0;
  }

  void SlicingViewMainDisplay::clearMarking()
  {
    mSampleView.setMarkedRegion(0, 0);
    mMarkState = 0;
  }

  bool SlicingViewMainDisplay::isMarked()
  {
    return mSampleView.mMarkStart < mSampleView.mMarkEnd;
  }

  float SlicingViewMainDisplay::getMarkedDuration()
  {
    if (mpHead == 0)
      return 0;

    Sample *pSample = mpHead->getSample();

    if (pSample == 0)
      return 0;

    return (mSampleView.mMarkEnd - mSampleView.mMarkStart) / pSample->mSampleRate;
  }

  float SlicingViewMainDisplay::getMarkedBeginTime()
  {
    if (mpHead == 0)
      return 0;

    Sample *pSample = mpHead->getSample();

    if (pSample == 0)
      return 0;

    return mSampleView.mMarkStart / pSample->mSampleRate;
  }

  float SlicingViewMainDisplay::getMarkedEndTime()
  {
    if (mpHead == 0)
      return 0;

    Sample *pSample = mpHead->getSample();

    if (pSample == 0)
      return 0;

    return mSampleView.mMarkEnd / pSample->mSampleRate;
  }

  void SlicingViewMainDisplay::showPeriodGadget()
  {
    encoderGridPeriod(0, false, 0);
    mActiveGadget = Gadget::periodGadget;
  }

  void SlicingViewMainDisplay::showDivisionGadget()
  {
    encoderGridDivision(0, false, 0);
    mActiveGadget = Gadget::divisionGadget;
  }

  void SlicingViewMainDisplay::showShiftGadget()
  {
    mSliceShiftInMS = 0;
    encoderSliceShift(0, false, 0.0f);
    mActiveGadget = Gadget::shiftGadget;
  }

  void SlicingViewMainDisplay::showOnsetGadget()
  {
    if (mpHead == 0)
      return;

    Sample *pSample = mpHead->getSample();
    Slices *pSlices = slices();

    if (pSample == 0 || pSlices == 0)
      return;

    if (mSampleView.mMarkStart >= mSampleView.mMarkEnd)
    {
      slicer.computeSpectralFlux(pSample, 0, pSample->mSampleCount, 500,
                                 0.5f);
    }
    else
    {
      slicer.computeSpectralFlux(pSample, mSampleView.mMarkStart,
                                 mSampleView.mMarkEnd, 500, 0.5f);
    }
    encoderOnsetThreshold(0, false, 0.0f);
    mActiveGadget = Gadget::onsetGadget;
  }

  void SlicingViewMainDisplay::hideGadget()
  {
    switch (mActiveGadget)
    {
    case Gadget::onsetGadget:
      slicer.freeResources();
      break;
    case Gadget::periodGadget:
      break;
    case Gadget::shiftGadget:
      break;
    default:
      break;
    }
    mActiveGadget = Gadget::none;
  }

} /* namespace od */
