/*
 * ProgressBar.cpp
 *
 *  Created on: 13 Oct 2016
 *      Author: clarkson
 */

#include <od/graphics/controls/ProgressBar.h>

namespace od
{

	ProgressBar::ProgressBar(int left, int bottom, int width, int height) : Graphic(left, bottom, width, height)
	{
	}

	ProgressBar::~ProgressBar()
	{
	}

	void ProgressBar::setTarget(Parameter &param)
	{
		mParameter.tie(param);
	}

	void ProgressBar::clearTarget()
	{
		mParameter.untie();
	}

	void ProgressBar::draw(FrameBuffer &fb)
	{
		Graphic::draw(fb);

		fb.box(GRAY3, mWorldLeft, mWorldBottom, mWorldLeft + mWidth - 1,
					 mWorldBottom + mHeight - 1);

		int w = mWidth * mParameter.value();
		w = MIN(MAX(w, 0), mWidth);

		fb.fill(WHITE, mWorldLeft, mWorldBottom, mWorldLeft + w - 1,
						mWorldBottom + mHeight - 1);
	}

} /* namespace od */
