#include <od/graphics/text/Utils.h>
#include <od/graphics/fonts.h>

namespace od
{

  int getCaretLocation(const char *text, int size, int position)
  {
    const font_t *font = 0;
    int c = 0, x = 0;

    while (text[c] != '\0' && position > 0)
    {
      font = font_lookup(size, (uint8_t)text[c]);
      // advance the draw position to the right of the current character
#if FONT_USE_WIDTH_TO_ADVANCE
      x += font->width + 1;
#else
      x += font->xadvance;
#endif
      c++;
      position--;
    }

    return x;
  }

  int getCharacterAdvance(char c, int size)
  {
    const font_t *font = 0;
    font = font_lookup(size, (int)c);
    if (font)
      return font->xadvance;
    else
      return 0;
  }

  int getCharacterWidth(char c, int size)
  {
    const font_t *font = 0;
    font = font_lookup(size, (int)c);
    if (font)
      return font->width + font->xoffset;
    else
      return 0;
  }

  int getStringAdvance(const char *text, int size)
  {
    const font_t *font = 0;
    int c = 0, x = 0;

    while (text[c] != '\0')
    {
      font = font_lookup(size, (uint8_t)text[c]);
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

  int getStringWidth(const char *text, int size)
  {
    const font_t *font = 0;
    int c = 0, x = 0;

    while (text[c] != '\0')
    {
      font = font_lookup(size, (uint8_t)text[c]);
      // advance the draw position to the right of the current character
#if FONT_USE_WIDTH_TO_ADVANCE
      x += font->width + 1;
#else
      x += font->xadvance;
#endif
      c++;
    }

    if (font)
    {
#if FONT_USE_WIDTH_TO_ADVANCE
      return x + font->width + font->xoffset - font->width + 1;
#else
      return x + font->width + font->xoffset - font->xadvance;
#endif
    }
    else
    {
      return x;
    }
  }

  void getTextSize(const char *text, int *width, int *height, int size)
  {
    const font_t *font = 0;
    int c = 0, x = 0;

    while (text[c] != '\0')
    {
      font = font_lookup(size, (uint8_t)text[c]);
      // advance the draw position to the right of the current character
#if FONT_USE_WIDTH_TO_ADVANCE
      x += font->width + 1;
#else
      x += font->xadvance;
#endif
      c++;
    }

    if (font)
    {
#if FONT_USE_WIDTH_TO_ADVANCE
      *width = x + font->width + font->xoffset - font->width + 1;
#else
      *width = x + font->width + font->xoffset - font->xadvance;
#endif
    }
    else
    {
      *width = 0;
    }

    // reference height provided by capital 'A'
    font = font_lookup(size, (uint8_t)'A');
    *height = font->height;
  }

  int getTextWidth(const char *text, int size)
  {
    int width, height;
    getTextSize(text, &width, &height, size);
    return width;
  }

  bool shortenText(std::string &text, int width, int fontSize)
  {
    int w, h;
    getTextSize(text.c_str(), &w, &h, fontSize);
    if (w <= width)
    {
      return false;
    }

    bool left = true;
    int n0 = text.size() / 2;
    int n1 = n0 + 2;
    text.replace(n0, 1, "..");
    while (w > width && n0 > 0 && n1 < (int)text.size())
    {
      if (left)
      {
        left = false;
        n0--;
        n1--;
        text.erase(n0, 1);
      }
      else
      {
        left = true;
        text.erase(n1, 1);
      }
      getTextSize(text.c_str(), &w, &h, fontSize);
    }
    return true;
  }

} // namespace od