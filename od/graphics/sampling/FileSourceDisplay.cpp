#include <od/graphics/sampling/FileSourceDisplay.h>
#include <stdio.h>

namespace od
{

  static void timeString(float totalSecs, Label &result)
  {
    int hours = totalSecs / 3600;
    totalSecs -= hours * 3600;
    int mins = totalSecs / 60;
    totalSecs -= mins * 60;
    int secs = totalSecs;
    totalSecs -= secs;
    int ms = 1000 * totalSecs;
    char tmp[32];
    snprintf(tmp, sizeof(tmp), "%d:%02d:%02d.%03d", hours, mins, secs, ms);
    result.setText(tmp);
  }

  FileSourceDisplay::FileSourceDisplay(int left, int bottom, int width,
                                       int height) : Graphic(left, bottom, width, height)
  {
    int h = height / 2 + 12;

    own(mFilenameLabel);
    addChild(&mFilenameLabel);
    mFilenameLabel.setPosition(0, h);
    mFilenameLabel.setSize(width, 12);

    h -= 12;
    own(mPositionLabel);
    addChild(&mPositionLabel);
    mPositionLabel.setPosition(0, h);
    mPositionLabel.setSize(width, 12);

    h -= 12;
    own(mDurationLabel);
    addChild(&mDurationLabel);
    mDurationLabel.setPosition(0, h);
    mDurationLabel.setSize(width, 12);

    h -= 12;
    own(mStatusLabel);
    addChild(&mStatusLabel);
    mStatusLabel.setPosition(0, h);
    mStatusLabel.setSize(width, 12);
  }

  FileSourceDisplay::~FileSourceDisplay()
  {
    if (mpSource)
    {
      mpSource->release();
    }
    mpSource = 0;
  }

  void FileSourceDisplay::setFileSource(FileSource *source)
  {
    if (mpSource)
    {
      mpSource->release();
    }
    mpSource = source;
    mLastPosition = 0;
    mLastDuration = 0;
    if (mpSource)
    {
      mpSource->attach();
    }
  }

  void FileSourceDisplay::setDisplayName(const std::string &name)
  {
    mFilenameLabel.setText(name);
  }

  void FileSourceDisplay::draw(FrameBuffer &fb)
  {
    if (mpSource)
    {
      uint32_t curPosition = mpSource->getPositionInSamples();
      if (mLastPosition != curPosition)
      {
        mLastPosition = curPosition;
        float totalSecs = mpSource->getPositionInSeconds();
        timeString(totalSecs, mPositionLabel);
      }

      uint32_t curDuration = mpSource->getDurationInSamples();
      if (mLastDuration != curDuration)
      {
        mLastDuration = curDuration;
        float totalSecs = mpSource->getDurationInSeconds();
        timeString(totalSecs, mDurationLabel);
      }

      if (mpSource->error())
      {
        mStatusLabel.setText(mpSource->getErrorString());
      }
      else if (mpSource->playing())
      {
        if (mpSource->buffering())
        {
          mStatusLabel.setText("Buffering...");
        }
        else
        {
          mStatusLabel.setText("Playing...");
        }

        float bufferMaxDepth = mpSource->getMaxBufferDepthInSeconds();
        float bufferDepth = mpSource->getBufferDepthInSeconds();
        int x = (mWidth - 20) * (bufferDepth / bufferMaxDepth);
        int y = mWorldBottom + mHeight - 5;
        fb.hline(WHITE, mWorldLeft + 10, mWorldLeft + 10 + x, y);
        fb.hline(GRAY7, mWorldLeft + 10 + x, mWorldLeft + mWidth - 10, y);
      }
      else
      {
        mStatusLabel.setText("Stopped.");
      }
    }
    Graphic::draw(fb);
  }

} /* namespace od */
