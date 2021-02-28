#pragma once

#include <od/graphics/Graphic.h>
#include <string>
#include <vector>

namespace od
{

  struct ListBoxItem
  {
    ListBoxItem(const std::string &name, const std::string &data,
                CheckState checked) : name(name), data(data), checked(checked)
    {
    }
    std::string name;
    std::string data;
    CheckState checked;
    bool highlight = false;
  };

  class ListBox : public Graphic
  {
  public:
    ListBox(int left, int bottom, int width, int height);
    virtual ~ListBox();
#ifndef SWIGLUA
    virtual void draw(FrameBuffer &fb);
#endif
    void addItem(const std::string &name);
    void addItem(const std::string &name, const std::string &data);
    void addItem(const std::string &name, const std::string &data,
                 CheckState checked);
    void addItem(const std::string &name, const std::string &data,
                 bool checked);
    void removeTopItem();
    void removeBottomItem();
    void removeItemByName(const std::string &name);
    void removeItemByData(const std::string &data);
    void checkSelected(CheckState checkState = checkYes);
    void uncheckSelected();
    void checkAll();
    void uncheckAll();
    bool updateByData(const std::string &newName, const std::string &newData,
                      const std::string &data);
    bool updateNameByData(const std::string &newName, const std::string &data);
    bool updateNameByIndex(const std::string &newName, int i);
    int getSelectedIndex()
    {
      return mSelectIndex;
    }
    const std::string &getSelected();
    const ListBoxItem &getSelectedItem();
    const std::string &getSelectedName();
    const std::string &getSelectedData();
    CheckState getSelectedCheckState();

    int find(const std::string &name);
    int findByData(const std::string& data);

    bool choose(const std::string &name);
    bool chooseWithData(const std::string &data);
    bool chooseByIndex(int i);
    void clear();
    void sortAscending();
    void sortDescending();

    bool highlight(const std::string &name);
    bool highlightWithData(const std::string &data);
    void showHighlight();
    void hideHighlight();

    bool scrollUp();
    bool scrollDown();
    void scrollToBottom();
    void scrollToTop();
    bool encoder(int change, bool shifted, int threshold);

    void showScrollbar();
    void hideScrollbar();

    void showSelection();
    void hideSelection();

    void setFocus();
    void clearFocus();

    void setLeftMargin(int x);
    void setTextSize(int size);

    void enableCircularScrolling();
    void disableCircularScrolling();

    void enableFollow();
    void disableFollow();

    void showCheck();
    void hideCheck();
    bool isCheckShown();

    void setEmptyText(const std::string &text);
    void setJustification(Justification style);

    int size();
    const std::string &get(int i);
    const ListBoxItem &getItem(int i);
    const std::string &getName(int i);
    const std::string &getData(int i);
    CheckState getCheckState(int i);
    void swap(int i, int j);

    int countCheckState(CheckState checkState = checkYes);

  protected:
    std::string mEmptyText{"Empty"};
    std::vector<ListBoxItem *> mContents;
    int mTopIndex, mSelectIndex;
    int mHeightInLines;
    int mTextSize = 12;
    int mMargin = 3;
    int mEncoderSum = 0;
    Justification mJustification = justifyLeft;
    bool mShowSelection = true;
    bool mShowScrollbar = true;
    bool mHasFocus = false;
    bool mCircularScrolling = false;
    bool mFollow = true;
    bool mShowCheck = false;
    bool mShowHighlight = false;

    void resetPositionAndIndices();
  };

} /* namespace od */

