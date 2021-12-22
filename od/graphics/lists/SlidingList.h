#pragma once

#include <od/graphics/Graphic.h>
#include <string>
#include <vector>

namespace od
{

  class SlidingList : public Graphic
  {
  public:
    SlidingList(int left, int bottom, int width, int height);
    virtual ~SlidingList();

#ifndef SWIGLUA
    virtual void draw(FrameBuffer &fb);
#endif

    void add(const std::string &text);
    void clear();
    int size();

    bool select(const std::string &name);
    bool select(int index);
    const std::string &selectedText();
    int selectedIndex();

    void scrollUp();
    void scrollDown();
    void scrollToBottom();
    void scrollToTop();
    bool encoder(int change, bool shifted, int threshold);

    void setTextSize(int size);
    void setJustification(Justification style);
    void setMargin(int margin);
    void setVerticalMargin(int margin);
    void learnDirection();
    void reverseDirection();
    void setSelectionDrawStyle(SelectionDrawStyle style);

  protected:
    std::vector<std::string> mContents;
    int mSelectedIndex = 0;
    int mTextSize = 10;
    int mVerticalMargin = 0;
    int mEncoderSum = 0;
    Justification mJustification = justifyLeft;
    int mMargin = 2;
    bool mReverseEncoder = false;
    bool mLearnDirection = true;

    SelectionDrawStyle mSelectionDrawStyle = straight;
  };

} /* namespace od */

