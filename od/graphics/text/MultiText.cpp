/*
 * MultiText.cpp
 *
 *  Created on: 8 May 2016
 *      Author: clarkson
 */

#include <od/graphics/text/MultiText.h>
#include <od/graphics/fonts.h>

namespace od
{

	MultiText::MultiText(int nlines, int left, int bottom, int width, int height) : Graphic(left, bottom, width, height), nlines(nlines)
	{

		lines = std::vector<std::string>(nlines);
		pos = nlines - 1;
		last = nlines - 1;
	}

	MultiText::~MultiText()
	{
	}

	void MultiText::clear()
	{
		lines.clear();
	}

	const std::string &MultiText::getLine(int pos)
	{
		if (pos < 0)
			return empty_line;
		int first = last + 1;

		if (first == nlines)
			first = 0;

		int i = first + pos;

		if (i < nlines)
		{
			return lines[i];
		}

		i -= nlines;

		if (i < nlines)
		{
			return lines[i];
		}

		return empty_line;
	}

	void MultiText::draw(FrameBuffer &fb)
	{
		int i, j, y;
		const font_family_t &font = *consolas12b_info(0);

		// render from line 'pos' and progress up until we leave the bounding box
		for (i = pos, j = 0; i >= 0; i--, j++)
		{
			y = mBottom + j * font.line_height + font.line_height - font.base;
			if (y < mBottom + mHeight)
			{
				fb.text(mForeground, mLeft, y, getLine(i).c_str());
			}
			else
			{
				break;
			}
		}
	}

	void MultiText::appendLine(const std::string &line)
	{
		last++;

		if (last == nlines)
		{
			last = 0;
		}

		lines[last] = line;
	}

} /* namespace od */
