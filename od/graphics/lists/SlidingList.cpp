#include <od/graphics/lists/SlidingList.h>
#include <od/graphics/fonts.h>
#include <stdlib.h>

namespace od
{

  SlidingList::SlidingList(int left, int bottom, int width, int height) : Graphic(left, bottom, width, height)
  {
    mCursorState.orientation = cursorRight;
  }

  SlidingList::~SlidingList()
  {
  }

  void SlidingList::draw(FrameBuffer &fb)
  {
    Graphic::draw(fb);
    if (mContents.size() == 0)
    {
      return;
    }

    int i, n = size();
    int mid = mWorldBottom + mHeight / 2;
    const font_family_t &family = *font_info(mTextSize, 0);
    int x, y;

    int lineHeight = family.line_height + mVerticalMargin;
    int limit = mWorldBottom - lineHeight;

    switch (mJustification)
    {
    case justifyLeft:
      // render down from selection
      x = mWorldLeft + mMargin;
      y = mid;
      for (i = mSelectedIndex; i < n; i++, y -= lineHeight)
      {
        if (y <= limit)
          break;
        fb.text(mForeground, x, y, mContents.at(i).c_str(), mTextSize,
                ALIGN_MIDDLE);
      }

      // render up from selection
      y = mid + lineHeight;
      limit = mWorldBottom + mHeight + lineHeight;
      for (i = mSelectedIndex - 1; i >= 0; i--, y += lineHeight)
      {
        if (y >= limit)
          break;
        fb.text(mForeground, x, y, mContents.at(i).c_str(), mTextSize,
                ALIGN_MIDDLE);
      }
      break;
    case justifyCenter:
      // render down from selection
      x = mWorldLeft;
      y = mid;
      for (i = mSelectedIndex; i < n; i++, y -= lineHeight)
      {
        if (y <= limit)
          break;
        const std::string &text = mContents.at(i);
        int textWidth, textHeight;
        getTextSize(text.c_str(), &textWidth, &textHeight, mTextSize);
        fb.text(mForeground, x + (mWidth - textWidth) / 2, y, text.c_str(),
                mTextSize,
                ALIGN_MIDDLE);
      }

      // render up from selection
      y = mid + lineHeight;
      limit = mWorldBottom + mHeight + lineHeight;
      for (i = mSelectedIndex - 1; i >= 0; i--, y += lineHeight)
      {
        if (y >= limit)
          break;
        const std::string &text = mContents.at(i);
        int textWidth, textHeight;
        getTextSize(text.c_str(), &textWidth, &textHeight, mTextSize);
        fb.text(mForeground, x + (mWidth - textWidth) / 2, y,
                mContents.at(i).c_str(), mTextSize,
                ALIGN_MIDDLE);
      }
      break;
    case justifyRight:
      // render down from selection
      x = mWorldLeft - mMargin;
      y = mid;
      for (i = mSelectedIndex; i < n; i++, y -= lineHeight)
      {
        if (y <= limit)
          break;
        const std::string &text = mContents.at(i);
        int textWidth, textHeight;
        getTextSize(text.c_str(), &textWidth, &textHeight, mTextSize);
        fb.text(mForeground, x + mWidth - textWidth, y, text.c_str(),
                mTextSize,
                ALIGN_MIDDLE);
      }

      // render up from selection
      y = mid + lineHeight;
      limit = mWorldBottom + mHeight + lineHeight;
      for (i = mSelectedIndex - 1; i >= 0; i--, y += lineHeight)
      {
        if (y >= limit)
          break;
        const std::string &text = mContents.at(i);
        int textWidth, textHeight;
        getTextSize(text.c_str(), &textWidth, &textHeight, mTextSize);
        fb.text(mForeground, x + mWidth - textWidth, y,
                mContents.at(i).c_str(), mTextSize,
                ALIGN_MIDDLE);
      }
      break;
    }

    // render selection highlight
    if (fb.mIsMonoChrome)
    {
      int y0 = mid - lineHeight / 2 - 1;
      int y1 = y0 + lineHeight;
      switch (mSelectionDrawStyle)
      {
      case SelectionDrawStyle::straight:
        fb.hline(mForeground, mWorldLeft, mWorldLeft + mWidth, y0);
        fb.hline(mForeground, mWorldLeft, mWorldLeft + mWidth, y1);
        break;
      case SelectionDrawStyle::box:
        fb.box(mForeground, mWorldLeft + 2, y0, mWorldLeft + mWidth - 2, y1);
        break;
      case SelectionDrawStyle::beveledBox:
        fb.drawBeveledBox(mForeground, mWorldLeft + 2, y0, mWorldLeft + mWidth - 2, y1);
        break;
      }
    }
    else
    {
      int y0 = mid - lineHeight / 2 - 1;
      int y1 = y0 + lineHeight;
      switch (mSelectionDrawStyle)
      {
      case SelectionDrawStyle::straight:
        fb.fill(GRAY3, mWorldLeft, y0, mWorldLeft + mWidth, y1);
        break;
      case SelectionDrawStyle::box:
        fb.box(mForeground, mWorldLeft + 2, y0, mWorldLeft + mWidth - 2, y1);
        break;
      case SelectionDrawStyle::beveledBox:
        fb.drawBeveledBox(mForeground, mWorldLeft + 2, y0, mWorldLeft + mWidth - 2, y1);
        break;
      }
    }

    mCursorState.x = mWorldLeft;
    mCursorState.y = mid;
  }

  void SlidingList::add(const std::string &text)
  {
    mContents.push_back(text);
  }

  void SlidingList::clear()
  {
    mContents.clear();
    mSelectedIndex = 0;
  }

  int SlidingList::size()
  {
    return (int)mContents.size();
  }

  bool SlidingList::select(const std::string &name)
  {
    int n = size();
    for (int i = 0; i < n; i++)
    {
      if (name == mContents.at(i))
      {
        mSelectedIndex = i;
        return true;
      }
    }
    return false;
  }

  bool SlidingList::select(int index)
  {
    if (index >= 0 && index < size())
    {
      mSelectedIndex = index;
      return true;
    }
    return false;
  }

  const std::string &SlidingList::selectedText()
  {
    static std::string empty{""};
    if (mContents.size() == 0)
    {
      return empty;
    }

    return mContents.at(mSelectedIndex);
  }

  int SlidingList::selectedIndex()
  {
    return mSelectedIndex;
  }

  void SlidingList::scrollUp()
  {
    if (mContents.size() == 0)
      return;
    mSelectedIndex = MAX(0, mSelectedIndex - 1);
  }

  void SlidingList::scrollDown()
  {
    if (mContents.size() == 0)
      return;
    mSelectedIndex = MIN(size() - 1, mSelectedIndex + 1);
  }

  void SlidingList::scrollToBottom()
  {
    if (mContents.size() == 0)
      return;
    mSelectedIndex = size() - 1;
  }

  void SlidingList::scrollToTop()
  {
    if (mContents.size() == 0)
      return;
    mSelectedIndex = 0;
  }

  bool SlidingList::encoder(int change, bool shifted, int threshold)
  {
    mEncoderSum += change;
    if (mLearnDirection && abs(mEncoderSum) > threshold)
    {
      mLearnDirection = false;
      if (mReverseEncoder)
      {
        if (mEncoderSum > 0)
        {
          if (mSelectedIndex == 0)
          {
            mReverseEncoder = false;
          }
        }
        else if (mEncoderSum < 0)
        {
          if (mSelectedIndex == size() - 1)
          {
            mReverseEncoder = false;
          }
        }
      }
      else
      {
        if (mEncoderSum > 0)
        {
          if (mSelectedIndex == size() - 1)
          {
            mReverseEncoder = true;
          }
        }
        else if (mEncoderSum < 0)
        {
          if (mSelectedIndex == 0)
          {
            mReverseEncoder = true;
          }
        }
      }
    }

    if (mReverseEncoder)
    {
      if (mEncoderSum > threshold)
      {
        mEncoderSum = 0;
        if (shifted)
        {
          scrollToTop();
        }
        else
        {
          scrollUp();
        }
        return true;
      }
      else if (mEncoderSum < -threshold)
      {
        mEncoderSum = 0;
        if (shifted)
        {
          scrollToBottom();
        }
        else
        {
          scrollDown();
        }
        return true;
      }
    }
    else
    {
      if (mEncoderSum > threshold)
      {
        mEncoderSum = 0;
        if (shifted)
        {
          scrollToBottom();
        }
        else
        {
          scrollDown();
        }
        return true;
      }
      else if (mEncoderSum < -threshold)
      {
        mEncoderSum = 0;
        if (shifted)
        {
          scrollToTop();
        }
        else
        {
          scrollUp();
        }
        return true;
      }
    }
    return false;
  }

  void SlidingList::setTextSize(int size)
  {
    mTextSize = size;
  }

  void SlidingList::setJustification(Justification style)
  {
    mJustification = style;
  }

  void SlidingList::setMargin(int margin)
  {
    mMargin = margin;
  }

  void SlidingList::setVerticalMargin(int margin)
  {
    mVerticalMargin = margin;
  }

  void SlidingList::learnDirection()
  {
    mLearnDirection = true;
  }

  void SlidingList::reverseDirection()
  {
    mReverseEncoder = true;
  }

  void SlidingList::setSelectionDrawStyle(SelectionDrawStyle style)
  {
    mSelectionDrawStyle = style;
  }

} /* namespace od */
