/*
 * BinaryIndicator.cpp
 *
 *  Created on: 26 Oct 2016
 *      Author: clarkson
 */

#include <od/graphics/controls/BinaryIndicator.h>

namespace od
{

	BinaryIndicator::BinaryIndicator(int left, int bottom, int width, int height) : Graphic(left, bottom, width, height)
	{
	}

	BinaryIndicator::~BinaryIndicator()
	{
	}

	void BinaryIndicator::draw(FrameBuffer &fb)
	{
		Graphic::draw(fb);

		int i, x, y, r;
		x = mWorldLeft + mWidth / 2;
		y = mWorldBottom + mHeight / 2;
		r = mWidth / 4;

		fb.circle(GRAY3, x, y, r);
		r += 2;
		int cost = r * WHITE * mState;
		cost /= 2;
		for (i = 1; i < r; i += 2)
		{
			cost -= WHITE;
			if (cost > WHITE)
			{
				fb.circle(WHITE, x, y, i);
			}
			else if (cost > 0)
			{
				fb.circle(cost, x, y, i);
			}
		}
	}

} /* namespace od */
