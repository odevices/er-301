#pragma once

#include <od/graphics/Graphic.h>
#include <od/graphics/sampling/SlicingViewMainDisplay.h>
#include <od/graphics/sampling/SliceList.h>

namespace od
{

  class SlicingViewSubDisplay : public Graphic
  {
  public:
    SlicingViewSubDisplay(SlicingViewMainDisplay *display);
    virtual ~SlicingViewSubDisplay();

#ifndef SWIGLUA
    virtual void draw(FrameBuffer &fb);
#endif

    void setName(const std::string &name);

    void setSlicesMode();
    void setCursorMode();
    void setFollowMode();

    int getSelectedSliceIndex();
    void syncPointerToSelectedSlice();
    void syncSelectedSliceToPointer();

    bool encoderSlices(int change, bool shifted, int threshold);

  protected:
    SlicingViewMainDisplay *mpMainDisplay = 0;
    SliceList mSliceList;
    uint32_t mLastHeadPosition = 0;
    uint32_t mLastCursorPosition = 0;
    float mLastDuration = 0;
    std::string mHeadText{"00:00.000"};
    std::string mCursorText{"00:00.000"};
    std::string mDurationText{"00:00.000"};
    std::string mName;

    enum Modes
    {
      slicesMode,
      cursorMode,
      followMode
    };

    Modes mMode = cursorMode;
  };

} /* namespace od */
