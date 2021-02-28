#include <od/graphics/FrameBuffer.h>

namespace od
{

  FrameBuffer::FrameBuffer()
  {
  }

  FrameBuffer::FrameBuffer(int w, int h) : mWidth(w), mHeight(h)
  {
  }

  FrameBuffer::~FrameBuffer()
  {
  }

  void FrameBuffer::drawUpTriangle(Color color, int x, int y, int height)
  {
    int x0 = x + 1;
    int x1 = x - 1;
    while (height-- > 0)
    {
      pixel(color, x, y);
      for (int r = 0; r < height; r++)
      {
        pixel(color, x0 + r, y);
        pixel(color, x1 - r, y);
      }
      y++;
    }
  }

  void FrameBuffer::drawDownTriangle(Color color, int x, int y, int height)
  {
    int x0 = x + 1;
    int x1 = x - 1;
    while (height-- > 0)
    {
      pixel(color, x, y);
      for (int r = 0; r < height; r++)
      {
        pixel(color, x0 + r, y);
        pixel(color, x1 - r, y);
      }
      y--;
    }
  }

  void FrameBuffer::drawLeftTriangle(Color color, int x, int y, int height)
  {
    int y0 = y + 1;
    int y1 = y - 1;
    while (height-- > 0)
    {
      pixel(color, x, y);
      for (int r = 0; r < height; r++)
      {
        pixel(color, x, y0 + r);
        pixel(color, x, y1 - r);
      }
      x--;
    }
  }

  void FrameBuffer::drawRightTriangle(Color color, int x, int y, int height)
  {
    int y0 = y + 1;
    int y1 = y - 1;
    while (height-- > 0)
    {
      pixel(color, x, y);
      for (int r = 0; r < height; r++)
      {
        pixel(color, x, y0 + r);
        pixel(color, x, y1 - r);
      }
      x++;
    }
  }

  void FrameBuffer::drawSpeaker(int x, int y)
  {
    vline(GRAY11, x, y + 3, y + 6);
    vline(WHITE, x + 1, y + 3, y + 6);
    vline(GRAY7, x + 2, y + 3, y + 6);
    vline(WHITE, x + 3, y + 2, y + 7);
    vline(WHITE, x + 4, y + 1, y + 8);
    vline(WHITE, x + 5, y, y + 9);

    vline(GRAY7, x + 6, y + 3, y + 6);

    vline(GRAY7, x + 9, y + 3, y + 6);
    pixel(GRAY7, x + 7, y);
    pixel(GRAY7, x + 7, y + 9);
    pixel(GRAY7, x + 8, y + 1);
    pixel(GRAY7, x + 8, y + 2);
    pixel(GRAY7, x + 8, y + 7);
    pixel(GRAY7, x + 8, y + 8);

    vline(GRAY3, x + 11, y + 3, y + 6);
    pixel(GRAY3, x + 9, y);
    pixel(GRAY3, x + 9, y + 9);
    pixel(GRAY3, x + 10, y + 1);
    pixel(GRAY3, x + 10, y + 2);
    pixel(GRAY3, x + 10, y + 7);
    pixel(GRAY3, x + 10, y + 8);
  }

  void FrameBuffer::drawEnterFolder(int x, int y)
  {
    fill(GRAY7, x + 4, y + 2, x + 13, y + 8);
    hline(GRAY7, x + 4, x + 8, y + 9);
    pixel(WHITE, x + 5, y + 4);
    vline(WHITE, x + 4, y + 3, y + 5);
    vline(WHITE, x + 3, y + 2, y + 6);
    hline(WHITE, x, x + 3, y + 4);
  }

  void FrameBuffer::drawLeaveFolder(int x, int y)
  {
    fill(GRAY7, x + 4, y + 2, x + 13, y + 8);
    hline(GRAY7, x + 4, x + 8, y + 9);
    pixel(WHITE, x, y + 4);
    vline(WHITE, x + 1, y + 3, y + 5);
    vline(WHITE, x + 2, y + 2, y + 6);
    hline(WHITE, x + 3, x + 7, y + 4);
  }

  void FrameBuffer::drawTab(Color color, int radius, int left, int bottom,
                            int right, int top)
  {
    // top border
    hline(color, left + radius, right - radius, top);
    // left border
    vline(color, left, bottom, top - radius);
    // right border
    vline(color, right, bottom, top - radius);

    // top-left corner
    arc8(color, left + radius, top - radius, radius, 0b00001100);
    // top-right corner
    arc8(color, right - radius, top - radius, radius, 0b00000011);
  }

  void FrameBuffer::drawBeveledBox(Color color, int left, int bottom,
                                   int right, int top)
  {
    const int radius = 2;
    // top border
    hline(color, left + radius, right - radius, top);
    // bottom border
    hline(color, left + radius, right - radius, bottom);
    // left border
    vline(color, left, bottom + radius, top - radius);
    // right border
    vline(color, right, bottom + radius, top - radius);

    // close the corners
    pixel(color, left + 1, top - 1);
    pixel(color, right - 1, top - 1);
    pixel(color, left + 1, bottom + 1);
    pixel(color, right - 1, bottom + 1);
  }
} /* namespace od */
