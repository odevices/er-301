#include <od/graphics/MainFrameBuffer.h>
#include <od/graphics/fonts.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

namespace od
{

  MainFrameBuffer::MainFrameBuffer()
  {
    mWidth = MAIN_SCREEN_WIDTH;
    mHeight = SCREEN_HEIGHT;
  }

  MainFrameBuffer::~MainFrameBuffer()
  {
  }

  void MainFrameBuffer::clear()
  {
    memset(frame, 0, sizeof(uint16_t) * 64 * 128);
  }

  // procedural version
  static inline void blend_pixel_proc(uint16_t *frame, Color color, int x, int y)
  {
    // row stride: 128
    // col stride: 1/2
    x = 255 - x;
    frame += (y << 7) + (x >> 1);
    *frame |= color << (((~x) & 0b1) << 2);
  }

  static inline void invert_pixel_proc(uint16_t *frame, int x, int y)
  {
    // row stride: 128
    // col stride: 1/2
    x = 255 - x;
    frame += (y << 7) + (x >> 1);
    int shift = ((~x) & 0b1) << 2;
    Color color = 15 - ((*frame >> shift) & 0xF);
    *frame &= ~(0xF << shift);
    *frame |= color << shift;
  }

  static inline void set_pixel_proc(uint16_t *frame, Color color, int x, int y)
  {
    // row stride: 128
    // col stride: 1/2
    x = 255 - x;
    frame += (y << 7) + (x >> 1);
    int shift = ((~x) & 0b1) << 2;
    *frame &= ~(0xF << shift);
    *frame |= color << shift;
  }

  static inline void clear_pixel_proc(uint16_t *frame, int x, int y)
  {
    // row stride: 128
    // col stride: 1/2
    x = 255 - x;
    *(frame + (y << 7) + (x >> 1)) &= ~(0xF << (((~x) & 0b1) << 2));
  }

  static inline void blend_pixel_safe(uint16_t *frame, Color color, int x, int y)
  {
    if (((x & 0xFF) == x) && ((y & 0x3F) == y))
      blend_pixel_proc(frame, color, x, y);
  }

  static inline void set_pixel_safe(uint16_t *frame, Color color, int x, int y)
  {
    if (((x & 0xFF) == x) && ((y & 0x3F) == y))
      set_pixel_proc(frame, color, x, y);
  }

  static inline void clear_pixel_safe(uint16_t *frame, int x, int y)
  {
    if (((x & 0xFF) == x) && ((y & 0x3F) == y))
      clear_pixel_proc(frame, x, y);
  }

  void MainFrameBuffer::pixel(Color color, int x, int y)
  {
    set_pixel_safe(frame, color, x, y);
  }

  void MainFrameBuffer::clear(int x, int y)
  {
    clear_pixel_safe(frame, x, y);
  }

  void MainFrameBuffer::blend(Color color, int x, int y)
  {
    blend_pixel_safe(frame, color, x, y);
  }

  void MainFrameBuffer::hlineSet(Color color, int x, int x2, int y)
  {
    if (y < 0 || y > mHeight - 1)
      return;

    int left = MIN(x, x2);
    int right = MAX(x, x2);

    if (left >= mWidth || right < 0)
      return;

    left = CLAMP(0, mWidth - 1, left);
    right = CLAMP(0, mWidth - 1, right);

    // solid
    for (; left <= right; left++)
    {
      set_pixel_safe(frame, color, left, y);
    }
  }

  void MainFrameBuffer::hline(Color color, int x, int x2, int y, int dotting)
  {
    if (y < 0 || y > mHeight - 1)
      return;

    int left = MIN(x, x2);
    int right = MAX(x, x2);

    if (left >= mWidth || right < 0)
      return;

    left = CLAMP(0, mWidth - 1, left);
    right = CLAMP(0, mWidth - 1, right);

    if (dotting > 0)
    {
      // dotted
      for (int count = dotting++; left <= right; left++, count--)
      {
        if (count > 0)
        {
          blend_pixel_safe(frame, color, left, y);
        }
        else
        {
          count = dotting;
        }
      }
    }
    else
    {
      // solid
      for (; left <= right; left++)
      {
        blend_pixel_safe(frame, color, left, y);
      }
    }
  }

  void MainFrameBuffer::vline(Color color, int x, int y, int y2, int dotting)
  {
    if (x < 0 || x > mWidth - 1)
      return;

    int bottom = MIN(y, y2);
    int top = MAX(y, y2);

    bottom = CLAMP(0, mHeight - 1, bottom);
    top = CLAMP(0, mHeight - 1, top);

    if (dotting > 0)
    {
      // dotted
      for (int count = dotting++; bottom <= top; bottom++, count--)
      {
        if (count > 0)
        {
          blend_pixel_safe(frame, color, x, bottom);
        }
        else
        {
          count = dotting;
        }
      }
    }
    else
    {
      // solid
      for (; bottom <= top; bottom++)
      {
        blend_pixel_safe(frame, color, x, bottom);
      }
    }
  }

  void MainFrameBuffer::line(Color color, int x, int y, int x2, int y2)
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
          blend_pixel_safe(frame, color, j >> 8, y);
          j += decInc;
        }
        return;
      }
      longLen += y;
      for (j = 0x80 + (x << 8); y >= longLen; --y)
      {
        blend_pixel_safe(frame, color, j >> 8, y);
        j -= decInc;
      }
      return;
    }

    if (longLen > 0)
    {
      longLen += x;
      for (j = 0x80 + (y << 8); x <= longLen; ++x)
      {
        blend_pixel_safe(frame, color, x, j >> 8);
        j += decInc;
      }
      return;
    }
    longLen += x;
    for (j = 0x80 + (y << 8); x >= longLen; --x)
    {
      blend_pixel_safe(frame, color, x, j >> 8);
      j -= decInc;
    }
  }

  void MainFrameBuffer::box(Color color, int left, int bottom, int right, int top)
  {
    hline(color, left, right, top);
    hline(color, left, right, bottom);
    vline(color, left, bottom, top);
    vline(color, right, bottom, top);
  }

  void MainFrameBuffer::clear(int left, int bottom, int right, int top)
  {
    right++;
    top++;

    if (top > 64)
      top = 64;
    if (bottom < 0)
      bottom = 0;
    if (left < 0)
      left = 0;
    if (right > 256)
      right = 256;

    int i, j;
    for (i = left; i < right; i++)
    {
      for (j = bottom; j < top; j++)
      {
        clear_pixel_proc(frame, i, j);
      }
    }
  }

  void MainFrameBuffer::fill(Color color, int left, int bottom, int right,
                             int top)
  {
    if (left > mWidth || bottom > mHeight || right < 0 || top < 0)
      return;

    right++;
    top++;

    if (top > 64)
      top = 64;
    if (bottom < 0)
      bottom = 0;
    if (left < 0)
      left = 0;
    if (right > 256)
      right = 256;

    int i, j;
    for (i = left; i < right; i++)
    {
      for (j = bottom; j < top; j++)
      {
        blend_pixel_proc(frame, color, i, j);
      }
    }
  }

  void MainFrameBuffer::invert(int left, int bottom, int right, int top)
  {
    if (left > mWidth || bottom > mHeight || right < 0 || top < 0)
      return;

    right++;
    top++;

    if (top > 64)
      top = 64;
    if (bottom < 0)
      bottom = 0;
    if (left < 0)
      left = 0;
    if (right > 256)
      right = 256;

    int i, j;
    for (i = left; i < right; i++)
    {
      for (j = bottom; j < top; j++)
      {
        invert_pixel_proc(frame, i, j);
      }
    }
  }

  int MainFrameBuffer::text(Color color, int x, int y, const char *text,
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
#if 0
      if(text[c]=='\xc2')
      {
        // Skip the UTF-8 prefix.
        c++;
        continue;
      }
#endif
      font = font_lookup(size, (uint8_t)text[c]);
      bitmap = font->bitmap;
      // draw font at x,y
      for (j = 0; j < font->height; j++)
      {
        yy = y + j + font->yoffset + v;
        for (i = 0; i < font->width; i++)
        {
          xx = x + i + font->xoffset;
          blend_pixel_safe(frame, (*bitmap) ? color : 0, xx, yy);
          //set_pixel_safe(frame, (*bitmap)>>4, xx, yy);
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

  int MainFrameBuffer::vtext(Color color, int x, int y, const char *text,
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
          blend_pixel_safe(frame, (*bitmap) ? color : 0, xx, yy);
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

  void MainFrameBuffer::circle(Color c, int xCenter, int yCenter, int radius)
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

  void MainFrameBuffer::fillCircle(Color c, int cx, int cy, int radius)
  {
    int x = 0;
    int y = radius;
    int p = (5 - radius * 4) / 4;

    hlineSet(c, cx - x, cx + x, cy + y);
    hlineSet(c, cx - x, cx + x, cy - y);
    hlineSet(c, cx - y, cx + y, cy + x);
    hlineSet(c, cx - y, cx + y, cy - x);

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

      hlineSet(c, cx - x, cx + x, cy + y);
      hlineSet(c, cx - x, cx + x, cy - y);
      hlineSet(c, cx - y, cx + y, cy + x);
      hlineSet(c, cx - y, cx + y, cy - x);
    }
  }

  void MainFrameBuffer::arc8(Color color, int x0, int y0, int radius, int octants)
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
