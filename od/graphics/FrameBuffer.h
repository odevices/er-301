#pragma once

#include <stdint.h>
#include <od/graphics/constants.h>
#include <od/graphics/primitives.h>

namespace od
{

  class FrameBuffer
  {
  public:
    FrameBuffer();
    FrameBuffer(int w, int h);
    virtual ~FrameBuffer();

    void setBuffer(uint16_t *ptr)
    {
      frame = ptr;
    }

    // Drawing primitives.

    virtual void clear() = 0;
    virtual void clear(int left, int bottom, int right, int top) = 0;
    virtual void blend(Color color, int x, int y) = 0;
    virtual void pixel(Color color, int x, int y) = 0;
    virtual void clear(int x, int y) = 0;
    virtual void line(Color color, int x0, int y0, int x1, int y1) = 0;
    virtual void hline(Color color, int x, int x2, int y, int dotting = 0) = 0;
    virtual void vline(Color color, int x, int y, int y2, int dotting = 0) = 0;
    virtual void box(Color color, int left, int bottom, int right, int top) = 0;
    virtual void fill(Color color, int left, int bottom, int right, int top) = 0;
    virtual void invert(int left, int bottom, int right, int top) = 0;
    virtual int text(Color color, int x, int y, const char *text,
                     int size = 12, int alignment = ALIGN_BASE) = 0;
    virtual int vtext(Color color, int x, int y, const char *text, int size = 12, int alignment = ALIGN_BASE) = 0;
    virtual void circle(Color color, int x, int y, int radius) = 0;
    virtual void fillCircle(Color color, int x, int y, int radius) = 0;
    virtual void arc8(Color color, int x, int y, int radius, int octants) = 0;

    //// Some common shapes.

    // (x,y) is the center of the base of the triangle
    void drawUpTriangle(Color color, int x, int y, int height);
    void drawDownTriangle(Color color, int x, int y, int height);
    void drawLeftTriangle(Color color, int x, int y, int height);
    void drawRightTriangle(Color color, int x, int y, int height);
    void drawSpeaker(int x, int y);
    void drawEnterFolder(int x, int y);
    void drawLeaveFolder(int x, int y);
    void drawTab(Color color, int radius, int left, int bottom, int right, int top);
    void drawBeveledBox(Color color, int left, int bottom, int right, int top);

    int mWidth, mHeight;
    bool mIsMonoChrome = false;

  protected:
    uint16_t *frame = 0;
  };

} /* namespace od */
