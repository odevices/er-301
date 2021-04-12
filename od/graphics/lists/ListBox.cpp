#include <od/graphics/lists/ListBox.h>
#include <od/graphics/fonts.h>
#include <hal/constants.h>
#include <algorithm>

namespace od
{

  ListBox::ListBox(int left, int bottom, int width, int height) : Graphic(left, bottom, width, height)
  {
    const font_family_t &font = *font_info_default(0);
    mHeightInLines = mHeight / font.line_height;
    mCursorState.orientation = cursorRight;
  }

  ListBox::~ListBox()
  {
    clear();
  }

  void ListBox::draw(FrameBuffer &fb)
  {
    Graphic::draw(fb);

    int y;
    int left, bottom, right, top, textLeft;
    left = mWorldLeft;
    bottom = mWorldBottom;
    right = left + mWidth;
    top = bottom + mHeight;

    textLeft = left + 2 + mMargin;

    if (fb.mIsMonoChrome)
    {
      // make room for selection triangle
      textLeft += 3;
    }

    if (mShowCheck)
    {
      // make room for checkbox
      textLeft += 8;
    }

    const font_family_t &family = *font_info(mTextSize, 0);

    if (mContents.size() == 0)
    {
      // no files
      fb.text(mForeground, textLeft,
              bottom + mHeight / 2 + family.line_height / 2,
              mEmptyText.c_str(), mTextSize, ALIGN_MIDDLE);
      return;
    }

    // draw selection
    if (mShowSelection)
    {
      int y0, y1;
      y0 = top - (mSelectIndex - mTopIndex + 1) * family.line_height;
      y1 = y0 + family.line_height - 1;
      mCursorState.x = left;
      mCursorState.y = (y0 + y1) / 2;
      if (mHasFocus)
      {
        if (fb.mIsMonoChrome)
        {
          fb.drawRightTriangle(WHITE, left, (y0 + y1) / 2, 5);
        }
        else if (mShowHighlight)
        {
          fb.box(mForeground, left + 2, y0, right - 4, y1);
        }
        else
        {
          fb.fill(GRAY3, left + 2, y0, right - 4, y1);
        }
      }
      else
      {
        if (fb.mIsMonoChrome)
        {
          fb.box(WHITE, left, y0, left + 3, y1);
        }
        else
        {
          fb.box(GRAY3, left + 2, y0, right - 4, y1);
        }
      }
    }

    // render from the top and progress down until we leave the bounding box
    int i, j;
    for (i = mTopIndex, j = 0, y = top - family.line_height / 2;
         i < size() && y >= bottom;
         i++, j++, y -= family.line_height)
    {
      const ListBoxItem &item = getItem(i);
      int textWidth, textHeight;
      getTextSize(item.name.c_str(), &textWidth, &textHeight, mTextSize);
      switch (mJustification)
      {
      case justifyLeft:
        if (mSelectIndex == i && textLeft + textWidth > right)
        {
          mLongTextSlideClock++;
          if (mLongTextSlideClock >= GRAPHICS_REFRESH_RATE / 5)
          {
            mLongTextSlideClock = 0;
            mLongTextSlide += mLongTextSlideDirection;
            if (mLongTextSlide < 1)
            {
              mLongTextSlideDirection = 1;
            }
            getTextSize(item.name.c_str() + mLongTextSlide, &textWidth, &textHeight, mTextSize);
            if (textLeft + textWidth < right)
            {
              mLongTextSlideDirection = -1;
            }
          }
          fb.text(mForeground, textLeft, y, item.name.c_str() + mLongTextSlide, mTextSize,
                  ALIGN_MIDDLE);
        }
        else
        {
          fb.text(mForeground, textLeft, y, item.name.c_str(), mTextSize,
                  ALIGN_MIDDLE);
        }

        break;
      case justifyCenter:
      {
        fb.text(mForeground, mWorldLeft + (mWidth - textWidth) / 2, y,
                item.name.c_str(), mTextSize,
                ALIGN_MIDDLE);
      }
      break;
      case justifyRight:
      {
        fb.text(mForeground, mWorldLeft + mWidth - textWidth - mMargin - 2,
                y, item.name.c_str(), mTextSize,
                ALIGN_MIDDLE);
      }
      break;
      }

      if (mShowCheck)
      {
        int l = textLeft - 8, b = y - 3, r = l + 5, t = b + 5;
        if (item.checked == checkYes)
        {
          fb.fill(mForeground, l, b, r, t);
        }
        else if (item.checked == checkPartial)
        {
          fb.fill(mForeground / 2, l, b, r, t);
        }
        else
        {
          //fb.clear(l, b, r, t);
          fb.box(GRAY7, l, b, r, t);
        }
      }

      if (mShowHighlight)
      {
        if (item.highlight)
        {
          int y0, y1;
          y0 = top - (j + 1) * family.line_height;
          y1 = y0 + family.line_height - 1;
          fb.fill(GRAY5, left + 2, y0, right - 4, y1);
        }
      }
    }

    if (mShowScrollbar)
    {
      // draw scroll bar
      float scrollSize, scrollPos;
      //scrollSize = (j - 1) / (float) mContents.size();
      scrollSize = j / (float)mContents.size();
      scrollPos = mTopIndex / (float)mContents.size();
      int scroll0, scroll1;
      scroll0 = top - scrollPos * mHeight;
      scroll1 = scroll0 - scrollSize * mHeight;
      fb.vline(mForeground, right - 2, scroll0, scroll1);
    }
  }

  bool ListBox::scrollUp()
  {
    resetLongTextSlide();
    if (mContents.size() == 0)
      return false;

    if (mSelectIndex > 0)
    {
      mSelectIndex--;
      if (mFollow && (mSelectIndex - mTopIndex < mHeightInLines / 2) && (mTopIndex > 0))
      {
        mTopIndex--;
      }
      return true;
    }
    else if (mCircularScrolling)
    {
      scrollToBottom();
      return true;
    }

    return false;
  }

  bool ListBox::scrollDown()
  {
    resetLongTextSlide();
    if (mContents.size() == 0)
      return false;

    if (mSelectIndex + 1 < (int)mContents.size())
    {
      mSelectIndex++;
      if (mFollow && (mSelectIndex - mTopIndex >= mHeightInLines / 2) && (mTopIndex + mHeightInLines < (int)mContents.size()))
      {
        mTopIndex++;
      }

      return true;
    }
    else if (mCircularScrolling)
    {
      scrollToTop();
      return true;
    }

    return false;
  }

  void ListBox::scrollToBottom()
  {
    resetLongTextSlide();
    if (mContents.size() == 0)
      return;

    while (mSelectIndex + 1 < (int)mContents.size())
    {
      scrollDown();
    }
  }

  void ListBox::scrollToTop()
  {
    resetLongTextSlide();
    if (mContents.size() == 0)
      return;

    resetPositionAndIndices();
  }

  bool ListBox::encoder(int change, bool shifted, int threshold)
  {
    mEncoderSum += change;
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
    return false;
  }

  void ListBox::addItem(const std::string &name)
  {
    addItem(name, "", checkNo);
  }

  void ListBox::addItem(const std::string &name, const std::string &data)
  {
    ListBoxItem *pItem = new ListBoxItem(name, data, checkNo);
    mContents.push_back(pItem);
    if (mContents.size() == 1)
    {
      // first item, so initialize pointers
      resetPositionAndIndices();
    }
  }

  void ListBox::addItem(const std::string &name, const std::string &data,
                        CheckState checked)
  {
    ListBoxItem *pItem = new ListBoxItem(name, data, checked);
    mContents.push_back(pItem);
    if (mContents.size() == 1)
    {
      // first item, so initialize pointers
      resetPositionAndIndices();
    }
  }

  void ListBox::addItem(const std::string &name, const std::string &data,
                        bool checked)
  {
    if (checked)
    {
      addItem(name, data, CheckState::checkYes);
    }
    else
    {
      addItem(name, data, CheckState::checkNo);
    }
  }

  void ListBox::removeItemByName(const std::string &name)
  {
    int i = find(name);
    if (i < size())
    {
      int save = getSelectedIndex();
      delete mContents[i];
      mContents.erase(mContents.begin() + i);
      resetPositionAndIndices();
      chooseByIndex(save);
    }
  }

  void ListBox::removeItemByData(const std::string &data)
  {
    int i = findByData(data);
    if (i < size())
    {
      int save = getSelectedIndex();
      delete mContents[i];
      mContents.erase(mContents.begin() + i);
      resetPositionAndIndices();
      chooseByIndex(save);
    }
  }

  void ListBox::removeTopItem()
  {
    if (size() > 0)
    {
      int save = getSelectedIndex();
      delete mContents[0];
      mContents.erase(mContents.begin());
      resetPositionAndIndices();
      chooseByIndex(save);
    }
  }

  void ListBox::removeBottomItem()
  {
    if (size() > 0)
    {
      int save = getSelectedIndex();
      delete mContents.back();
      mContents.pop_back();
      resetPositionAndIndices();
      chooseByIndex(save);
    }
  }

  bool ListBox::updateNameByData(const std::string &newName,
                                 const std::string &data)
  {
    int i = findByData(data);
    if (i < size())
    {
      mContents[i]->name = newName;
      return true;
    }
    return false;
  }

  bool ListBox::updateNameByIndex(const std::string &newName,
                                  int i)
  {
    if (i >= 0 && i < size())
    {
      mContents[i]->name = newName;
      return true;
    }
    return false;
  }

  bool ListBox::updateByData(const std::string &newName, const std::string &newData,
                             const std::string &data)
  {
    int i = findByData(data);
    if (i < size())
    {
      ListBoxItem *p = mContents[i];
      p->name = newName;
      p->data = newData;
      return true;
    }
    return false;
  }

  void ListBox::clear()
  {
    for (ListBoxItem *p : mContents)
    {
      delete p;
    }
    mContents.clear();
    resetPositionAndIndices();
  }

  const std::string &ListBox::get(int i)
  {
    return getItem(i).name;
  }

  const ListBoxItem &ListBox::getItem(int i)
  {
    static ListBoxItem empty{"", "", checkNo};
    if (i < 0 || i >= size())
      return empty;
    return *(mContents[i]);
  }

  const std::string &ListBox::getName(int i)
  {
    return getItem(i).name;
  }

  const std::string &ListBox::getData(int i)
  {
    return getItem(i).data;
  }

  CheckState ListBox::getCheckState(int i)
  {
    return getItem(i).checked;
  }

  void ListBox::checkSelected(CheckState checkState)
  {
    mContents[mSelectIndex]->checked = checkState;
  }

  void ListBox::uncheckSelected()
  {
    mContents[mSelectIndex]->checked = checkNo;
  }

  const ListBoxItem &ListBox::getSelectedItem()
  {
    return getItem(mSelectIndex);
  }

  const std::string &ListBox::getSelected()
  {
    return getItem(mSelectIndex).name;
  }

  const std::string &ListBox::getSelectedName()
  {
    return getItem(mSelectIndex).name;
  }

  const std::string &ListBox::getSelectedData()
  {
    return getItem(mSelectIndex).data;
  }

  CheckState ListBox::getSelectedCheckState()
  {
    return getSelectedItem().checked;
  }

  int ListBox::find(const std::string &name)
  {
    int n = size();
    for (int i = 0; i < n; i++)
    {
      if (mContents[i]->name == name)
      {
        return i;
      }
    }
    return n;
  }

  int ListBox::findByData(const std::string &data)
  {
    int n = size();
    for (int i = 0; i < n; i++)
    {
      if (mContents[i]->data == data)
      {
        return i;
      }
    }
    return n;
  }

  bool ListBox::choose(const std::string &name)
  {
    scrollToTop();
    for (const ListBoxItem *p : mContents)
    {
      if (name == p->name)
      {
        return true;
      }
      scrollDown();
    }
    return false;
  }

  bool ListBox::chooseByIndex(int i)
  {
    scrollToTop();
    while (mSelectIndex != i && mSelectIndex != size() - 1)
    {
      scrollDown();
    }
    return false;
  }

  bool ListBox::chooseWithData(const std::string &data)
  {
    scrollToTop();
    for (const ListBoxItem *p : mContents)
    {
      if (data == p->data)
      {
        return true;
      }
      scrollDown();
    }
    return false;
  }

  static std::string tolower(std::string s)
  {
    std::transform(s.begin(), s.end(), s.begin(),
                   [](unsigned char c) { return std::tolower(c); });
    return s;
  }

  static bool ascending(ListBoxItem *a, ListBoxItem *b)
  {
    return tolower(a->name) < tolower(b->name);
  }

  static bool descending(ListBoxItem *a, ListBoxItem *b)
  {
    return tolower(a->name) > tolower(b->name);
  }

  void ListBox::sortAscending()
  {
    std::sort(mContents.begin(), mContents.end(), ascending);
    resetPositionAndIndices();
  }

  void ListBox::sortDescending()
  {
    std::sort(mContents.begin(), mContents.end(), descending);
    resetPositionAndIndices();
  }

  void ListBox::resetPositionAndIndices()
  {
    mSelectIndex = mTopIndex = 0;
  }

  void ListBox::checkAll()
  {
    for (ListBoxItem *p : mContents)
    {
      p->checked = checkYes;
    }
  }

  void ListBox::uncheckAll()
  {
    for (ListBoxItem *p : mContents)
    {
      p->checked = checkNo;
    }
  }

  bool ListBox::highlight(const std::string &name)
  {
    int i = find(name);
    if (i < size())
    {
      mContents[i]->highlight = true;
      return true;
    }
    return false;
  }

  bool ListBox::highlightWithData(const std::string &data)
  {
    int i = findByData(data);
    if (i < size())
    {
      mContents[i]->highlight = true;
      return true;
    }
    return false;
  }

  void ListBox::showHighlight()
  {
    mShowHighlight = true;
  }

  void ListBox::hideHighlight()
  {
    mShowHighlight = false;
  }

  void ListBox::showScrollbar()
  {
    mShowScrollbar = true;
  }

  void ListBox::hideScrollbar()
  {
    mShowScrollbar = false;
  }

  void ListBox::showSelection()
  {
    mShowSelection = true;
  }

  void ListBox::hideSelection()
  {
    mShowSelection = false;
  }

  void ListBox::setFocus()
  {
    mHasFocus = true;
  }

  void ListBox::clearFocus()
  {
    mHasFocus = false;
  }

  void ListBox::setLeftMargin(int x)
  {
    mMargin = x;
  }

  int ListBox::size()
  {
    return (int)mContents.size();
  }

  void ListBox::setTextSize(int size)
  {
    mTextSize = size;
  }

  void ListBox::enableCircularScrolling()
  {
    mCircularScrolling = true;
  }

  void ListBox::disableCircularScrolling()
  {
    mCircularScrolling = false;
  }

  void ListBox::enableFollow()
  {
    mFollow = true;
  }

  void ListBox::disableFollow()
  {
    mFollow = false;
  }

  void ListBox::showCheck()
  {
    mShowCheck = true;
  }

  void ListBox::hideCheck()
  {
    mShowCheck = false;
  }

  bool ListBox::isCheckShown()
  {
    return mShowCheck;
  }

  void ListBox::setEmptyText(const std::string &text)
  {
    mEmptyText = text;
  }

  void ListBox::setJustification(Justification style)
  {
    mJustification = style;
  }

  int ListBox::countCheckState(CheckState checkState)
  {
    int count = 0;
    for (ListBoxItem *p : mContents)
    {
      if (p->checked == checkState)
      {
        count++;
      }
    }
    return count;
  }

  void ListBox::swap(int i, int j)
  {
    ListBoxItem *p = mContents[i];
    mContents[i] = mContents[j];
    mContents[j] = p;
  }

  void ListBox::resetLongTextSlide()
  {
    mLongTextSlideClock = 0;
    mLongTextSlideDirection = 1;
    mLongTextSlide = 0;
  }

} /* namespace od */
