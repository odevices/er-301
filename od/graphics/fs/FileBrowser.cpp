#include <od/graphics/fs/FileBrowser.h>
#include <od/extras/Utils.h>
#include <hal/card.h>
#include <hal/fileops.h>
#include <hal/dir.h>
//#define BUILDOPT_VERBOSE
//#define BUILDOPT_DEBUG_LEVEL 10
#include <hal/log.h>
#include <sstream>
#include <algorithm>
#include <stdio.h>

#define COLLAPSED_FOLDER_WIDTH 15

namespace od
{

  FileBrowser::FileBrowser(int left, int bottom, int width, int height)
      : Graphic(left, bottom, width, height),
        mListBox(COLLAPSED_FOLDER_WIDTH, 0, width - COLLAPSED_FOLDER_WIDTH, height),
        mLeftMeter(mWidth - 14, mWorldBottom, 4, 36),
        mRightMeter(mWidth - 8, mWorldBottom, 4, 36)
  {
    own(mListBox);
    mListBox.setFocus();
    mListBox.setTextSize(10);
    addChild(&mListBox);
    own(mLeftMeter);
    addChild(&mLeftMeter);
    mLeftMeter.hide();
    own(mRightMeter);
    addChild(&mRightMeter);
    mRightMeter.hide();
  }

  FileBrowser::~FileBrowser()
  {
    setFileSource(0);
  }

  void FileBrowser::notifyEjected()
  {
    mEjected = true;
    mListBox.hide();
  }

  void FileBrowser::notifyMounted()
  {
    if (!pathExists(getSelectedPath().c_str()))
    {
      while (popDirectory())
      {
      }
    }
    refresh();
    mEjected = false;
    mListBox.show();
  }

  bool FileBrowser::refresh()
  {
    mEjected = Card_isMounted(1);
    if (mEjected)
    {
      mListBox.hide();
    }
    else
    {
      mListBox.show();
    }
    int i = mStack.size() + 1;
    if (i > 5)
      i = 5;
    mListBox.setPosition(i * COLLAPSED_FOLDER_WIDTH, 0);
    mListBox.setSize(mWidth - i * COLLAPSED_FOLDER_WIDTH, mHeight);

    bool result = buildFileList(getSelectedPath());

    if (result)
    {
      // set the focused item
      if (mFocusedItem == "")
      {
        scrollToTop();
        return true;
      }
      else
      {
        return choose(mFocusedItem);
      }
    }
    else
    {
      return false;
    }
  }

  bool FileBrowser::choose(const std::string &item)
  {
    mFocusedItem = item;
    if (mListBox.chooseWithData(mFocusedItem))
    {
      onSelectionChanged();
      return true;
    }
    else
    {
      onSelectionChanged();
      return false;
    }
  }

  void FileBrowser::updatePath()
  {
    std::ostringstream oss;
    oss << mRootPath;
    for (std::string &dir : mStack)
    {
      oss << '/';
      oss << dir;
    }
    mPath = oss.str();
    mPathDirty = false;
  }

  const std::string &FileBrowser::getSelectedPath()
  {
    if (mPathDirty)
    {
      updatePath();
    }
    return mPath;
  }

  const std::string &FileBrowser::getSelectedFullPath()
  {
    if (mPathDirty)
    {
      updatePath();
    }

    std::ostringstream oss;
    oss << mPath;
    oss << '/';
    oss << getSelectedName();
    mFullPath = oss.str();

    return mFullPath;
  }

  void FileBrowser::pushDirectory(const std::string &dir)
  {
    mStack.push_back(dir);
    mFocusedItem = "";
    mPathDirty = true;
  }

  bool FileBrowser::popDirectory()
  {
    if (mStack.size() > 0)
    {
      mFocusedItem = mStack.back();
      mStack.pop_back();
      mPathDirty = true;
      return true;
    }
    return false;
  }

  void FileBrowser::setRootPath(const std::string &path)
  {
    mRootPath = path;
    mStack.clear();
    mFocusedItem = "";
    mPathDirty = true;
  }

  void FileBrowser::clearPatterns()
  {
    mPatterns.clear();
    mPathDirty = true;
  }

  void FileBrowser::addPattern(const std::string &pattern)
  {
    mPatterns.push_back(pattern);
    mPathDirty = true;
  }

  void FileBrowser::setPattern(const std::string &pattern)
  {
    clearPatterns();
    addPattern(pattern);
  }

  bool FileBrowser::isDirectory()
  {
    if (mListBox.size() == 0)
      return false;
    const ListBoxItem &item = mListBox.getSelectedItem();
    return item.name != item.data;
  }

  bool FileBrowser::isSoundFile()
  {
    if (!isFile())
      return false;
    const ListBoxItem &item = mListBox.getSelectedItem();
    return glob(item.name.c_str(), "*.WAV");
  }

  bool FileBrowser::isChecked()
  {
    if (mListBox.size() == 0)
      return false;
    const ListBoxItem &item = mListBox.getSelectedItem();
    return item.checked == CheckState::checkYes;
  }

  bool FileBrowser::isFile()
  {
    if (mListBox.size() == 0)
      return false;
    const ListBoxItem &item = mListBox.getSelectedItem();
    return item.name == item.data;
  }

  bool FileBrowser::buildFileList(const std::string &path)
  {
    std::string pname, fname;
    CheckState checkState;
    char *tmp;
    uint32_t attributes;
    dir_t dir;

    mListBox.clear();

    dir = Dir_open(path.c_str());
    if (dir == 0)
      return false;

    while (Dir_read(dir, &tmp, &attributes))
    {
      if (attributes & (FILEOPS_HID | FILEOPS_SYS))
      {
        // ignore hidden and system files
        continue;
      }
      else if (tmp[0] == '.')
      {
        // ignore dot files
        continue;
      }
      fname = tmp;
      if (attributes & FILEOPS_DIR)
      {
        std::ostringstream ss;
        ss << '[' << fname << ']';
        pname = ss.str();
        checkState = getDirectoryCheckState(path, fname);
        mListBox.addItem(pname, fname, checkState);
      }
      else if (!mShowOnlyFolders)
      {
        if (glob(fname.c_str(), mPatterns))
        {
          pname = fname;
          checkState = getFileCheckState(path, fname);
          mListBox.addItem(pname, fname, checkState);
        }
      }
    }

    Dir_close(dir);

    mListBox.sortAscending();
    return true;
  }

  void FileBrowser::draw(FrameBuffer &fb)
  {
    int left = mWorldLeft;
    int mid = mWorldLeft + COLLAPSED_FOLDER_WIDTH / 2;

    if (mEjected && Card_isMounted(1))
    {
      notifyMounted();
    }
    else if (!mEjected && !Card_isMounted(1))
    {
      notifyEjected();
    }

    if (mStack.size() < 5)
    {
      fb.vtext(WHITE, mid, mWorldBottom + 3, mRootPath.c_str(), 12,
               ALIGN_MIDDLE);
      left += COLLAPSED_FOLDER_WIDTH;
      mid += COLLAPSED_FOLDER_WIDTH;
      fb.vline(WHITE, left, mWorldBottom, mWorldBottom + mHeight);

      for (const std::string &dir : mStack)
      {
        fb.vtext(WHITE, mid, mWorldBottom + 3, dir.c_str(), 12,
                 ALIGN_MIDDLE);
        left += COLLAPSED_FOLDER_WIDTH;
        mid += COLLAPSED_FOLDER_WIDTH;
        fb.vline(WHITE, left, mWorldBottom, mWorldBottom + mHeight);
      }
    }
    else
    {
      std::vector<std::string>::iterator iter;

      for (iter = mStack.end() - 5; iter != mStack.end(); iter++)
      {
        fb.vtext(WHITE, mid, mWorldBottom + 3, (*iter).c_str(), 12,
                 ALIGN_MIDDLE);
        left += COLLAPSED_FOLDER_WIDTH;
        mid += COLLAPSED_FOLDER_WIDTH;
        fb.vline(WHITE, left, mWorldBottom, mWorldBottom + mHeight);
      }
    }

    if (mEjected)
    {
      fb.text(WHITE, mListBox.mWorldLeft + 10, mWorldBottom + 32,
              "Card ejected.");
    }

    // draw children
    Graphic::draw(fb);

    int b5 = mWorldLeft + BUTTON5_CENTER;
    int b6 = mWorldLeft + BUTTON6_CENTER;

    // outline for folder navigation
    fb.clear(b5 - 5, mWorldBottom, b6 + 5, mWorldBottom + 14);
    fb.drawTab(GRAY7, 3, b5 - 5, mWorldBottom, b6 + 5, mWorldBottom + 14);
    fb.vline(GRAY5, mWorldLeft + COL6_LEFT, mWorldBottom, mWorldBottom + 11);

    if (mStack.size() > 0)
    {
      fb.drawLeaveFolder(b5, mWorldBottom + 1);
    }

    if (isDirectory())
    {
      fb.drawEnterFolder(b6 - 15, mWorldBottom + 1);
    }
    else if (isSoundFile())
    {
      bool playing = mpFileSource && mpFileSource->playing() && !mpFileSource->isEOF();
      if (playing || mStickyPlay)
      {
        fb.fill(WHITE, b6 - 12, mWorldBottom + 2, b6 - 4,
                mWorldBottom + 10);
        // play position
        fb.clear(b5 - 5, mWorldBottom + 25, b6 + 5, mWorldBottom + 35);
        fb.drawTab(GRAY7, 3, b5 - 5, mWorldBottom + 25, b6 + 5,
                   mWorldBottom + 35);
        int x = b5 - 3;
        int w = BUTTON6_CENTER + 5 - (BUTTON5_CENTER - 5);
        updatePlayPosition();
        fb.text(WHITE, x + (w - mPlayPositionWidth) / 2, mWorldBottom + 27,
                mPlayPosition.c_str(), ALIGN_BOTTOM);
      }
      else
      {
        fb.drawSpeaker(b6 - 13, mWorldBottom + 1);
      }
      if (mHaveSoundFileInfo)
      {
        fb.clear(b5 - 5, mWorldBottom + 15, b6 + 5, mWorldBottom + 25);
        fb.drawTab(GRAY7, 3, b5 - 5, mWorldBottom + 15, b6 + 5,
                   mWorldBottom + 25);
        int x = b5 - 3;
        int w = BUTTON6_CENTER + 5 - (BUTTON5_CENTER - 5);
        fb.text(WHITE, x + (w - mSoundFileInfoWidth) / 2, mWorldBottom + 16,
                mSoundFileInfo.c_str(), ALIGN_BOTTOM);
      }
    }
#if 0
    if (mListBox.isCheckShown())
    {
      int b4 = mWorldLeft + BUTTON4_CENTER;
      fb.clear(b4 - 14, mWorldBottom, b4 + 14, mWorldBottom + 14);
      fb.drawTab(GRAY7, 3, b4 - 14, mWorldBottom, b4 + 14,
                 mWorldBottom + 14);

      if (isChecked())
      {
        fb.fill(mForeground, b4 - 2, mWorldBottom + 3, b4 + 3,
                mWorldBottom + 8);
      }
      else
      {
        fb.box(GRAY7, b4 - 2, mWorldBottom + 3, b4 + 3, mWorldBottom + 8);
      }
    }
#endif
  }

  bool FileBrowser::encoder(int change, bool shifted, int threshold)
  {
    if (mListBox.encoder(change, shifted, threshold))
    {
      onSelectionChanged();
      return true;
    }
    else
    {
      return false;
    }
  }

  void FileBrowser::updatePlayPosition()
  {
    uint32_t curPosition = mpFileSource->getPositionInSamples();
    if (mLastPlayPosition != curPosition)
    {
      mLastPlayPosition = curPosition;
      float totalSecs = mpFileSource->getPositionInSeconds();
      int hours = totalSecs / 3600;
      int mins = (totalSecs - hours * 3600) / 60;
      int secs = totalSecs - mins * 60;
      char tmp[32];
      snprintf(tmp, sizeof(tmp), "%01d:%02d:%02d", hours, mins, secs);
      mPlayPosition = tmp;
      getTextSize(tmp, &mPlayPositionWidth, &mPlayPositionHeight, 10);
    }
  }

  void FileBrowser::onSelectionChanged()
  {
    if (isSoundFile())
    {
      mHaveSoundFileInfo = true;
      if (mWavFileReader.open(getSelectedFullPath()))
      {
        char tmp[64];
        float totalSecs = mWavFileReader.getTotalSeconds();
        int channelCount = (int)mWavFileReader.getChannelCount();
        if (totalSecs < 0.001f)
        {
          snprintf(tmp, sizeof(tmp), "%dch <1ms", channelCount);
        }
        else if (totalSecs < 1.0f)
        {
          snprintf(tmp, sizeof(tmp), "%dch %03.0fms", channelCount,
                   totalSecs * 1000);
        }
        else if (totalSecs < 60)
        {
          snprintf(tmp, sizeof(tmp), "%dch %02.1fs", channelCount,
                   totalSecs);
        }
        else if (totalSecs < 3600)
        {
          int mins = (int)(totalSecs / 60);
          int secs = (int)(totalSecs - mins * 60);
          snprintf(tmp, sizeof(tmp), "%dch %d:%02d", channelCount, mins,
                   secs);
        }
        else
        {
          int hours = (int)(totalSecs / 3600);
          totalSecs -= hours * 3600;
          int mins = (int)(totalSecs / 60);
          totalSecs -= mins * 60;
          int secs = (int)totalSecs;
          snprintf(tmp, sizeof(tmp), "%dch %d:%02d:%02d", channelCount,
                   hours, mins, secs);
        }

        mSoundFileInfo = tmp;
        getTextSize(tmp, &mSoundFileInfoWidth, &mSoundFileInfoHeight, 10);
        mWavFileReader.close();
      }
      else
      {
        mSoundFileInfo = "No info.";
        getTextSize("No info.", &mSoundFileInfoWidth, &mSoundFileInfoHeight,
                    10);
      }
    }
    else
    {
      mHaveSoundFileInfo = false;
    }
  }

  CheckState FileBrowser::getFileCheckState(const std::string &path,
                                            const std::string &filename)
  {
    std::ostringstream oss;
    oss << path << '/' << filename;
    if (mCheckedFiles.count(oss.str()) > 0)
    {
      return checkYes;
    }

    return checkNo;
  }

  CheckState FileBrowser::getDirectoryCheckState(const std::string &path,
                                                 const std::string &filename)
  {
    std::ostringstream oss;
    oss << path << '/' << filename;
    logDebug(1, "%s: %d", oss.str().c_str(), mCheckedPerDirectory[oss.str()]);
    if (mCheckedPerDirectory[oss.str()] > 0)
    {
      return checkPartial;
    }

    return checkNo;
  }

  void FileBrowser::clearChecked()
  {
    mCheckedPerDirectory.clear();
    mCheckedPerDirectory.clear();
    mListBox.uncheckAll();
  }

  void FileBrowser::checkNone()
  {
    const ListBoxItem &item = mListBox.getSelectedItem();
    mListBox.scrollToTop();
    uncheckSelected();
    while (mListBox.scrollDown())
    {
      uncheckSelected();
    }
    mListBox.chooseWithData(item.data);
  }

  void FileBrowser::checkAll()
  {
    const ListBoxItem &item = mListBox.getSelectedItem();
    mListBox.scrollToTop();
    checkSelected();
    while (mListBox.scrollDown())
    {
      checkSelected();
    }
    mListBox.chooseWithData(item.data);
  }

  int FileBrowser::checkDirectory(const std::string &path)
  {
    char *tmp;
    uint32_t attributes;
    std::string fullpath, fname;
    dir_t dir;
    int checked = 0;

    dir = Dir_open(path.c_str());
    if (dir == 0)
      return 0;

    while (Dir_read(dir, &tmp, &attributes))
    {
      if (attributes & (FILEOPS_HID | FILEOPS_SYS))
      {
        // ignore hidden and system files
        continue;
      }
      else if (tmp[0] == '.')
      {
        // ignore dot files
        continue;
      }

      fname = tmp;
      if (attributes & FILEOPS_DIR)
      {
        std::ostringstream ss;
        ss << path << '/' << fname;
        checked += checkDirectory(ss.str());
      }
      else if (glob(fname.c_str(), mPatterns))
      {
        std::ostringstream ss;
        ss << path << '/' << fname;
        fullpath = ss.str();
        if (mCheckedFiles.count(fullpath) == 0)
        {
          mCheckedFiles.emplace(fullpath);
          checked++;
        }
      }
    }

    Dir_close(dir);
    mCheckedPerDirectory[path] += checked;
    return checked;
  }

  int FileBrowser::uncheckDirectory(const std::string &path)
  {
    char *tmp;
    uint32_t attributes;
    std::string fullpath, fname;
    dir_t dir;
    int unchecked = 0;

    dir = Dir_open(path.c_str());
    if (dir == 0)
      return 0;

    while (Dir_read(dir, &tmp, &attributes))
    {
      if (attributes & (FILEOPS_HID | FILEOPS_SYS))
      {
        // ignore hidden and system files
        continue;
      }
      else if (tmp[0] == '.')
      {
        // ignore dot files
        continue;
      }

      fname = tmp;
      if (attributes & FILEOPS_DIR)
      {
        std::ostringstream ss;
        ss << path << '/' << fname;
        unchecked += uncheckDirectory(ss.str());
      }
      else if (glob(fname.c_str(), mPatterns))
      {
        std::ostringstream ss;
        ss << path << '/' << fname;
        fullpath = ss.str();
        if (mCheckedFiles.erase(fullpath) > 0)
        {
          unchecked++;
        }
      }
    }

    Dir_close(dir);
    mCheckedPerDirectory[path] -= unchecked;
    return unchecked;
  }

  void FileBrowser::checkSelected()
  {
    if (isFile())
    {
      mListBox.checkSelected();
      const std::string &selected = getSelectedFullPath();
      if (mCheckedFiles.count(selected) == 0)
      {
        mCheckedFiles.emplace(selected);
        std::ostringstream oss;
        oss << mRootPath;
        for (std::string &dir : mStack)
        {
          oss << '/';
          oss << dir;
          mCheckedPerDirectory[oss.str()]++;
        }
      }
    }
    else
    {
      mListBox.checkSelected(checkPartial);
      const std::string &selected = getSelectedFullPath();
      int checked = checkDirectory(selected);
      if (checked > 0)
      {
        std::ostringstream oss;
        oss << mRootPath;
        for (std::string &dir : mStack)
        {
          oss << '/';
          oss << dir;
          mCheckedPerDirectory[oss.str()] += checked;
        }
      }
    }
  }

  void FileBrowser::uncheckSelected()
  {
    if (isFile())
    {
      mListBox.uncheckSelected();
      const std::string &selected = getSelectedFullPath();
      if (mCheckedFiles.erase(selected) > 0)
      {
        std::ostringstream oss;
        oss << mRootPath;
        for (std::string &dir : mStack)
        {
          oss << '/';
          oss << dir;
          mCheckedPerDirectory[oss.str()]--;
        }
      }
    }
    else
    {
      mListBox.uncheckSelected();
      const std::string &selected = getSelectedFullPath();
      int unchecked = uncheckDirectory(selected);
      if (unchecked > 0)
      {
        std::ostringstream oss;
        oss << mRootPath;
        for (std::string &dir : mStack)
        {
          oss << '/';
          oss << dir;
          mCheckedPerDirectory[oss.str()] -= unchecked;
        }
      }
    }
  }

  void FileBrowser::toggleSelected()
  {
    if (isFile())
    {
      if (getFileCheckState(getSelectedPath(), getSelectedName()) == checkNo)
      {
        checkSelected();
      }
      else
      {
        uncheckSelected();
      }
    }
    else
    {
      if (getDirectoryCheckState(getSelectedPath(), getSelectedName()) == checkNo)
      {
        checkSelected();
      }
      else
      {
        uncheckSelected();
      }
    }
  }

  std::vector<std::string> FileBrowser::getCheckedPaths()
  {
    std::vector<std::string> result;

    for (const std::string &path : mCheckedFiles)
    {
      result.emplace_back(path);
    }

#if 0
    for (const std::string &path : mCheckedDirectories)
    {
      listMatchingFiles(path, true, mPatterns, result);
    }
#endif
    return result;
  }

  void FileBrowser::setFileSource(FileSource *source, bool sticky)
  {
    if (mpFileSource)
    {
      mLeftMeter.clearOutlet();
      mRightMeter.clearOutlet();
      mpFileSource->release();
      mLeftMeter.hide();
      mRightMeter.hide();
    }

    mpFileSource = source;
    if (mpFileSource)
    {
      mpFileSource->attach();
      if (mpFileSource->getOutputCount() == 2)
      {
        mLeftMeter.watchOutlet(mpFileSource->getOutput("Left Out"));
        mRightMeter.watchOutlet(mpFileSource->getOutput("Right Out"));
        mLeftMeter.show();
        mRightMeter.show();
      }
      else
      {
        mLeftMeter.watchOutlet(mpFileSource->getOutput("Out"));
        mLeftMeter.show();
      }
    }

    mStickyPlay = sticky;
  }

} /* namespace od */
