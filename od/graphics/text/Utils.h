#pragma once

#include <string>

namespace od
{
  int getCaretLocation(const char *text, int size, int position);
  void getTextSize(const char *text, int *width, int *height, int size);
  int getTextWidth(const char *text, int size);
  int getCharacterAdvance(char c, int size);
  int getCharacterWidth(char c, int size);
  int getStringAdvance(const char *text, int size);
  int getStringWidth(const char *text, int size);
  bool shortenText(std::string &text, int width, int size);
} /* namespace od */
