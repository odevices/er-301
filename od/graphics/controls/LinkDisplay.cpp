/*
 * LinkDisplay.cpp
 *
 *  Created on: 12 Aug 2016
 *      Author: clarkson
 */

#include <od/graphics/controls/LinkDisplay.h>

namespace od
{

	LinkDisplay::LinkDisplay() : Graphic(0, 0, 256, 64)
	{
	}

	LinkDisplay::~LinkDisplay()
	{
	}

#define RADIUS 27

	static inline void f(FrameBuffer &fb, Color color, int x0, int x1, int r0,
											 int r1, int bottom)
	{
		r1++;
		if (x0 == x1)
		{
			for (int r = r0; r < r1; r++)
			{
				fb.circle(color, x0, 32 + bottom, r);
			}
		}
		else
		{
			for (int r = r0; r < r1; r++)
			{
				fb.arc8(color, x0, 32 + bottom, r, 0b00111100);
				fb.arc8(color, x1, 32 + bottom, r, 0b11000011);
				fb.hline(color, x0, x1, 32 + r + bottom);
				fb.hline(color, x0, x1, 32 - r + bottom);
				fb.pixel(color, x0 - r, 32 + bottom);
				fb.pixel(color, x1 + r, 32 + bottom);
			}
		}
	}

	void LinkDisplay::draw(FrameBuffer &fb)
	{
		mCursorState.x = mWorldLeft + mSelection;
		mCursorState.y = mWorldBottom + mHeight;

		fb.text(WHITE, mWorldLeft + 32 - 3, mWorldBottom + 32, "1", 12, ALIGN_MIDDLE);
		fb.text(WHITE, mWorldLeft + 96 - 3, mWorldBottom + 32, "2", 12, ALIGN_MIDDLE);
		fb.text(WHITE, mWorldLeft + 160 - 3, mWorldBottom + 32, "3", 12, ALIGN_MIDDLE);
		fb.text(WHITE, mWorldLeft + 224 - 3, mWorldBottom + 32, "4", 12, ALIGN_MIDDLE);

		switch (mLinks)
		{
		case 0: // 1 2 3 4
			f(fb, GRAY11, 32, 32, RADIUS, RADIUS, mWorldBottom);
			f(fb, GRAY11, 96, 96, RADIUS, RADIUS, mWorldBottom);
			f(fb, GRAY11, 160, 160, RADIUS, RADIUS, mWorldBottom);
			f(fb, GRAY11, 224, 224, RADIUS, RADIUS, mWorldBottom);
			break;
		case 1: // 12 3 4
			f(fb, GRAY11, 32, 96, RADIUS, RADIUS, mWorldBottom);
			f(fb, GRAY11, 160, 160, RADIUS, RADIUS, mWorldBottom);
			f(fb, GRAY11, 224, 224, RADIUS, RADIUS, mWorldBottom);
			break;
		case 2: // 1 23 4
			f(fb, GRAY11, 32, 32, RADIUS, RADIUS, mWorldBottom);
			f(fb, GRAY11, 96, 160, RADIUS, RADIUS, mWorldBottom);
			f(fb, GRAY11, 224, 224, RADIUS, RADIUS, mWorldBottom);
			break;
		case 3: // 123 4
			f(fb, GRAY11, 32, 160, RADIUS, RADIUS, mWorldBottom);
			f(fb, GRAY11, 224, 224, RADIUS, RADIUS, mWorldBottom);
			break;
		case 4: // 1 2 34
			f(fb, GRAY11, 32, 32, RADIUS, RADIUS, mWorldBottom);
			f(fb, GRAY11, 96, 96, RADIUS, RADIUS, mWorldBottom);
			f(fb, GRAY11, 160, 224, RADIUS, RADIUS, mWorldBottom);
			break;
		case 5: // 12 34
			f(fb, GRAY11, 32, 96, RADIUS, RADIUS, mWorldBottom);
			f(fb, GRAY11, 160, 224, RADIUS, RADIUS, mWorldBottom);
			break;
		case 6: // 1 234
			f(fb, GRAY11, 32, 32, RADIUS, RADIUS, mWorldBottom);
			f(fb, GRAY11, 96, 224, RADIUS, RADIUS, mWorldBottom);
			break;
		case 7: // 1234
			f(fb, GRAY11, 32, 224, RADIUS, RADIUS, mWorldBottom);
			break;
		}
	}

	void LinkDisplay::link(int a, int b)
	{
		if (a > b)
		{
			int tmp;
			tmp = a;
			a = b;
			b = tmp;
		}

		if (a == 1 && b == 2)
		{
			mLinks |= 1;
		}
		else if (a == 2 && b == 3)
		{
			mLinks |= 1 << 1;
		}
		else if (a == 3 && b == 4)
		{
			mLinks |= 1 << 2;
		}
	}

	void LinkDisplay::unlink(int a, int b)
	{
		if (a > b)
		{
			int tmp;
			tmp = a;
			a = b;
			b = tmp;
		}

		if (a == 1 && b == 2)
		{
			mLinks &= ~(1);
		}
		else if (a == 2 && b == 3)
		{
			mLinks &= ~(1 << 1);
		}
		else if (a == 3 && b == 4)
		{
			mLinks &= ~(1 << 2);
		}
	}

} /* namespace od */
