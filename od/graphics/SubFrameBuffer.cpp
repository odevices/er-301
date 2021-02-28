#include <od/graphics/SubFrameBuffer.h>
#include <od/graphics/fonts.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

namespace od
{

  SubFrameBuffer::SubFrameBuffer()
  {
    mWidth = SUB_SCREEN_WIDTH;
    mHeight = SCREEN_HEIGHT;
    mIsMonoChrome = true;
  }

  SubFrameBuffer::~SubFrameBuffer()
  {
  }

  void SubFrameBuffer::clear()
  {
    memset(frame, 0, sizeof(uint16_t) * 8 * 128);
  }

  // procedural version
  static inline void set_pixel_proc(uint16_t *frame, Color color, int x, int y)
  {
    // column stride: 1
    // row stride: 128/8
    frame += ((y >> 3) << 7) + 127 - x;
    *frame |= (color > 0) << (y & 0b111);
  }

  static inline void invert_pixel_proc(uint16_t *frame, int x, int y)
  {
    // column stride: 1
    // row stride: 128/8
    frame += ((y >> 3) << 7) + 127 - x;
    *frame ^= 1 << (y & 0b111);
  }

  static inline void clear_pixel_proc(uint16_t *frame, int x, int y)
  {
    // column stride: 1
    // row stride: 128/8
    //y = 63-y;
    frame += ((y >> 3) << 7) + 127 - x;
    *frame &= ~(1 << (y & 0b111));
  }

  static inline void set_pixel_safe(uint16_t *frame, Color color, int x, int y)
  {
    if (((x & 0x7F) == x) && ((y & 0x3F) == y))
      set_pixel_proc(frame, color, x, y);
  }

  static inline void clear_pixel_safe(uint16_t *frame, int x, int y)
  {
    if (((x & 0x7F) == x) && ((y & 0x3F) == y))
      clear_pixel_proc(frame, x, y);
  }

  void SubFrameBuffer::pixel(Color color, int x, int y)
  {
    set_pixel_safe(frame, color, x, y);
  }

  void SubFrameBuffer::clear(int x, int y)
  {
    clear_pixel_safe(frame, x, y);
  }

  void SubFrameBuffer::blend(Color color, int x, int y)
  {
    set_pixel_safe(frame, color, x, y);
  }

  void SubFrameBuffer::hline(Color color, int x, int x2, int y, int dotting)
  {
    if (y < 0 || y > mHeight - 1)
      return;

    int left = MIN(x, x2);
    int right = MAX(x, x2);

    left = CLAMP(0, mWidth - 1, left);
    right = CLAMP(0, mWidth - 1, right);

    if (color < WHITE || dotting > 0)
    {
      // dotted
      bool state = true;
      for (; left <= right; left++)
      {
        if (state)
        {
          set_pixel_proc(frame, color, left, y);
        }
        state = !state;
      }
    }
    else
    {
      // solid
      for (; left <= right; left++)
      {
        set_pixel_proc(frame, color, left, y);
      }
    }
  }

  void SubFrameBuffer::vline(Color color, int x, int y, int y2, int dotting)
  {
    if (x < 0 || x > mWidth - 1)
      return;

    int bottom = MIN(y, y2);
    int top = MAX(y, y2);

    bottom = CLAMP(0, mHeight - 1, bottom);
    top = CLAMP(0, mHeight - 1, top);

    if (color < WHITE || dotting > 0)
    {
      // dotted
      bool state = true;
      for (; bottom <= top; bottom++)
      {
        if (state)
        {
          set_pixel_proc(frame, color, x, bottom);
        }
        state = !state;
      }
    }
    else
    {
      // solid
      for (; bottom <= top; bottom++)
      {
        set_pixel_proc(frame, color, x, bottom);
      }
    }
  }

  void SubFrameBuffer::line(Color color, int x, int y, int x2, int y2)
  {
    int j;
    bool yLonger = false;
    int shortLen = y2 - y;
    int longLen = x2 - x;
    if (abs(shortLen) > abs(longLen))
    {
      int swap = shortLen;
      shortLen = longLen;
      longLen = swap;
      yLonger = true;
    }
    int decInc;
    if (longLen == 0)
      decInc = 0;
    else
      decInc = (shortLen << 8) / longLen;

    if (yLonger)
    {
      if (longLen > 0)
      {
        longLen += y;
        for (j = 0x80 + (x << 8); y <= longLen; ++y)
        {
          set_pixel_safe(frame, color, j >> 8, y);
          j += decInc;
        }
        return;
      }
      longLen += y;
      for (j = 0x80 + (x << 8); y >= longLen; --y)
      {
        set_pixel_safe(frame, color, j >> 8, y);
        j -= decInc;
      }
      return;
    }

    if (longLen > 0)
    {
      longLen += x;
      for (j = 0x80 + (y << 8); x <= longLen; ++x)
      {
        set_pixel_safe(frame, color, x, j >> 8);
        j += decInc;
      }
      return;
    }
    longLen += x;
    for (j = 0x80 + (y << 8); x >= longLen; --x)
    {
      set_pixel_safe(frame, color, x, j >> 8);
      j -= decInc;
    }
  }

  void SubFrameBuffer::box(Color color, int left, int bottom, int right,
                           int top)
  {
    hline(color, left, right, top);
    hline(color, left, right, bottom);
    vline(color, left, bottom, top);
    vline(color, right, bottom, top);
  }

  void SubFrameBuffer::fill(Color color, int left, int bottom, int right,
                            int top)
  {
    if (color < WHITE || left > mWidth || bottom > mHeight || right < 0 || top < 0)
      return;

    top++;
    right++;

    if (top > 64)
      top = 64;
    if (bottom < 0)
      bottom = 0;
    if (left < 0)
      left = 0;
    if (right > 128)
      right = 128;

    int i, j;
    for (i = left; i < right; i++)
    {
      for (j = bottom; j < top; j++)
      {
        set_pixel_proc(frame, color, i, j);
      }
    }
  }

  void SubFrameBuffer::invert(int left, int bottom, int right, int top)
  {
    if (left > mWidth || bottom > mHeight || right < 0 || top < 0)
      return;

    top++;
    right++;

    if (top > 64)
      top = 64;
    if (bottom < 0)
      bottom = 0;
    if (left < 0)
      left = 0;
    if (right > 128)
      right = 128;

    int i, j;
    for (i = left; i < right; i++)
    {
      for (j = bottom; j < top; j++)
      {
        invert_pixel_proc(frame, i, j);
      }
    }
  }

  void SubFrameBuffer::clear(int left, int bottom, int right, int top)
  {
    top++;
    right++;

    if (top > 64)
      top = 64;
    if (bottom < 0)
      bottom = 0;
    if (left < 0)
      left = 0;
    if (right > 128)
      right = 128;

    int i, j;
    for (i = left; i < right; i++)
    {
      for (j = bottom; j < top; j++)
      {
        clear_pixel_proc(frame, i, j);
      }
    }
  }

  int SubFrameBuffer::text(Color color, int x, int y, const char *text, int size,
                           int alignment)
  {
    const font_t *font;
    const uint8_t *bitmap;
    int c = 0, i, j, xx, yy;
    int v = 0;
    //const font_family_t * family = font_info(size, 0);
    font = font_lookup(size, (uint8_t)'A');

    switch (alignment)
    {
    case ALIGN_TOP:
      //v = -family->base;
      v = -font->height;
      break;
    case ALIGN_MIDDLE:
      //v = family->line_height / 2 - family->base;
      v = -font->height / 2;
      break;
    case ALIGN_BOTTOM:
      //v = family->line_height - family->base;
      v = 0;
      break;
    case ALIGN_BASE:
      v = 0;
      break;
    }

    while (text[c] != '\0')
    {
      font = font_lookup(size, (uint8_t)text[c]);
      bitmap = font->bitmap;
      // draw font at x,y
      for (j = 0; j < font->height; j++)
      {
        yy = y + j + font->yoffset + v;
        for (i = 0; i < font->width; i++)
        {
          xx = x + i + font->xoffset;
          set_pixel_safe(frame, (*bitmap) >> 7, xx, yy);
          bitmap++;
        }
      }

      // advance the draw position to the right of the current character
#if FONT_USE_WIDTH_TO_ADVANCE
      x += font->width + 1;
#else
      x += font->xadvance;
#endif
      c++;
    }
    return x;
  }

  int SubFrameBuffer::vtext(Color color, int x, int y, const char *text,
                            int size, int alignment)
  {
    const font_t *font;
    const uint8_t *bitmap;
    int c = 0, i, j, xx, yy;
    int v = 0;
    //const font_family_t * family = font_info(size, 0);
    font = font_lookup(size, (uint8_t)'A');

    switch (alignment)
    {
    case ALIGN_TOP:
      //v = -family->base;
      v = -font->height;
      break;
    case ALIGN_MIDDLE:
      //v = family->line_height / 2 - family->base;
      v = -font->height / 2;
      break;
    case ALIGN_BOTTOM:
      //v = family->line_height - family->base;
      v = 0;
      break;
    case ALIGN_BASE:
      v = 0;
      break;
    }

    while (text[c] != '\0')
    {
      font = font_lookup(size, (uint8_t)text[c]);
      bitmap = font->bitmap;
      // draw font at x,y
      for (j = 0; j < font->height; j++)
      {
        xx = x - (j + font->yoffset + v);
        for (i = 0; i < font->width; i++)
        {
          yy = y + i + font->xoffset;
          set_pixel_safe(frame, (*bitmap) > 0 ? color : 0, xx, yy);
          bitmap++;
        }
      }

      // advance the draw position to the right of the current character
#if FONT_USE_WIDTH_TO_ADVANCE
      y += font->width + 1;
#else
      y += font->xadvance;
#endif
      c++;
    }

    return y;
  }

  static inline void circlePoints(uint16_t *frame, int cx, int cy, int x, int y,
                                  Color color)
  {
    if (x == 0)
    {
      set_pixel_safe(frame, color, cx, cy + y);
      set_pixel_safe(frame, color, cx, cy - y);
      set_pixel_safe(frame, color, cx + y, cy);
      set_pixel_safe(frame, color, cx - y, cy);
    }
    else if (x == y)
    {
      set_pixel_safe(frame, color, cx + x, cy + y);
      set_pixel_safe(frame, color, cx - x, cy + y);
      set_pixel_safe(frame, color, cx + x, cy - y);
      set_pixel_safe(frame, color, cx - x, cy - y);
    }
    else if (x < y)
    {
      set_pixel_safe(frame, color, cx + x, cy + y);
      set_pixel_safe(frame, color, cx - x, cy + y);
      set_pixel_safe(frame, color, cx + x, cy - y);
      set_pixel_safe(frame, color, cx - x, cy - y);
      set_pixel_safe(frame, color, cx + y, cy + x);
      set_pixel_safe(frame, color, cx - y, cy + x);
      set_pixel_safe(frame, color, cx + y, cy - x);
      set_pixel_safe(frame, color, cx - y, cy - x);
    }
  }

  void SubFrameBuffer::circle(Color c, int xCenter, int yCenter, int radius)
  {
    int x = 0;
    int y = radius;
    int p = (5 - radius * 4) / 4;

    circlePoints(frame, xCenter, yCenter, x, y, c);
    while (x < y)
    {
      x++;
      if (p < 0)
      {
        p += 2 * x + 1;
      }
      else
      {
        y--;
        p += 2 * (x - y) + 1;
      }
      circlePoints(frame, xCenter, yCenter, x, y, c);
    }
  }

  void SubFrameBuffer::fillCircle(Color c, int cx, int cy, int radius)
  {
    int x = 0;
    int y = radius;
    int p = (5 - radius * 4) / 4;

    hline(c, cx - x, cx + x, cy + y);
    hline(c, cx - x, cx + x, cy - y);
    hline(c, cx - y, cx + y, cy + x);
    hline(c, cx - y, cx + y, cy - x);

    while (x < y)
    {
      x++;
      if (p < 0)
      {
        p += 2 * x + 1;
      }
      else
      {
        y--;
        p += 2 * (x - y) + 1;
      }

      hline(c, cx - x, cx + x, cy + y);
      hline(c, cx - x, cx + x, cy - y);
      hline(c, cx - y, cx + y, cy + x);
      hline(c, cx - y, cx + y, cy - x);
    }
  }

  void SubFrameBuffer::arc8(Color color, int x0, int y0, int radius,
                            int octants)
  {
    int x = 0;
    int y = radius;
    int p = (5 - radius * 4) / 4;

    while (x < y)
    {
      x++;
      if (p < 0)
      {
        p += 2 * x + 1;
      }
      else
      {
        y--;
        p += 2 * (x - y) + 1;
      }

      if (octants & (1 << 1))
        set_pixel_safe(frame, color, x0 + x, y0 + y); // 45 to 90
      if (octants & (1 << 0))
        set_pixel_safe(frame, color, x0 + y, y0 + x); // 0 to 45

      if (octants & (1 << 3))
        set_pixel_safe(frame, color, x0 - y, y0 + x); // 135 to 180
      if (octants & (1 << 2))
        set_pixel_safe(frame, color, x0 - x, y0 + y); // 90 to 135

      if (octants & (1 << 5))
        set_pixel_safe(frame, color, x0 - x, y0 - y); // -90 to -135
      if (octants & (1 << 4))
        set_pixel_safe(frame, color, x0 - y, y0 - x); // -135 to -180

      if (octants & (1 << 7))
        set_pixel_safe(frame, color, x0 + y, y0 - x); // 0 to -45
      if (octants & (1 << 6))
        set_pixel_safe(frame, color, x0 + x, y0 - y); // -45 to -90
    }
  }

} // namespace od
/* namespace od */
