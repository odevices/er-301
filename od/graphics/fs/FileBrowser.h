#pragma once

#include <od/graphics/lists/ListBox.h>
#include <od/graphics/meters/VerticalMeter.h>
#include <od/audio/WavFileReader.h>
#include <od/objects/file/FileSource.h>
#include <set>
#include <vector>
#include <map>

namespace od
{

  class FileBrowser : public Graphic
  {
  public:
    FileBrowser(int left, int bottom, int width, int height);
    virtual ~FileBrowser();
#ifndef SWIGLUA
    virtual void draw(FrameBuffer &fb);
#endif
    void setRootPath(const std::string &path);

    void setPattern(const std::string &pattern);
    void addPattern(const std::string &pattern);
    void clearPatterns();

    void pushDirectory(const std::string &dir);
    bool popDirectory();
    bool refresh();
    bool choose(const std::string &item);

    void notifyEjected();
    void notifyMounted();

    void scrollUp()
    {
      mListBox.scrollUp();
      onSelectionChanged();
    }

    void scrollDown()
    {
      mListBox.scrollDown();
      onSelectionChanged();
    }

    void scrollToBottom()
    {
      mListBox.scrollToBottom();
      onSelectionChanged();
    }

    void scrollToTop()
    {
      mListBox.scrollToTop();
      onSelectionChanged();
    }

    bool encoder(int change, bool shifted, int threshold);

    void showSelection()
    {
      mListBox.showSelection();
    }

    void hideSelection()
    {
      mListBox.hideSelection();
    }

    void setFocus()
    {
      mListBox.setFocus();
    }

    void clearFocus()
    {
      mListBox.clearFocus();
    }

    void showOnlyFolders(bool value)
    {
      mShowOnlyFolders = value;
      refresh();
    }

    // just the path portion
    const std::string &getSelectedPath();
    // full path to the selected item
    const std::string &getSelectedFullPath();
    // just the last portion of the selected path
    const std::string &getSelectedName()
    {
      return mListBox.getSelectedData();
    }

    bool isDirectory();
    bool isFile();
    bool isSoundFile();
    bool isChecked();
    bool isEmpty()
    {
      return mListBox.size() == 0;
    }

    void showCheck()
    {
      mListBox.showCheck();
    }

    void hideCheck()
    {
      mListBox.hideCheck();
    }

    void clearChecked();
    void checkSelected();
    void uncheckSelected();
    void toggleSelected();
    void checkNone();
    void checkAll();
    std::vector<std::string> getCheckedPaths();
    int countCheckedPaths()
    {
      return (int)mCheckedFiles.size();
    }

    void setFileSource(FileSource *source, bool sticky = false);

  protected:
    ListBox mListBox;
    std::string mRootPath;
    std::string mPath;
    std::string mFullPath;
    std::vector<std::string> mPatterns;
    std::vector<std::string> mStack;
    std::string mFocusedItem;
    bool mPathDirty = true;
    std::set<std::string> mCheckedFiles;
    std::map<std::string, int> mCheckedPerDirectory;
    bool mEjected = true;
    bool mShowOnlyFolders = false;

    bool buildFileList(const std::string &path);
    void updatePath();
    CheckState getFileCheckState(const std::string &path,
                                 const std::string &filename);
    CheckState getDirectoryCheckState(const std::string &path,
                                      const std::string &filename);   
    int checkDirectory(const std::string& path);                                 
    int uncheckDirectory(const std::string& path);

    WavFileReader mWavFileReader;
    VerticalMeter mLeftMeter;
    VerticalMeter mRightMeter;
    bool mHaveSoundFileInfo = false;
    bool mStickyPlay = false;
    std::string mSoundFileInfo;
    int mSoundFileInfoWidth = 0;
    int mSoundFileInfoHeight = 0;
    FileSource *mpFileSource = 0;
    std::string mPlayPosition;
    int mPlayPositionWidth = 0;
    int mPlayPositionHeight = 0;
    uint32_t mLastPlayPosition = 0;
    void onSelectionChanged();
    void updatePlayPosition();
  };

} /* namespace od */
