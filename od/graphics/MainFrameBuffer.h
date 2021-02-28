#pragma once

#include <od/graphics/FrameBuffer.h>
#include <vector>

namespace od
{

  class MainFrameBuffer : public FrameBuffer
  {
  public:
    MainFrameBuffer();
    virtual ~MainFrameBuffer();

    void clear();
    void clear(int left, int bottom, int right, int top);
    void clear(int x, int y);
    void pixel(Color color, int x, int y);
    void blend(Color color, int x, int y);
    void line(Color color, int x0, int y0, int x1, int y1);
    void hline(Color color, int x, int x2, int y, int dotting = 0);
    void vline(Color color, int x, int y, int y2, int dotting = 0);
    void box(Color color, int left, int bottom, int right, int top);
    void fill(Color color, int left, int bottom, int right, int top);
    void invert(int left, int bottom, int right, int top);
    int text(Color color, int x, int y, const char *text, int size = 12, int alignment = ALIGN_BASE);
    int vtext(Color color, int x, int y, const char *text, int size = 12, int alignment = ALIGN_BASE);
    void circle(Color color, int x, int y, int radius);
    void fillCircle(Color color, int x, int y, int radius);
    virtual void arc8(Color color, int x, int y, int radius, int octants);

  private:
    void hlineSet(Color color, int x, int x2, int y);
  };

} /* namespace od */
