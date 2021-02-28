/*
 * StripChart.cpp
 *
 *  Created on: 23 Aug 2016
 *      Author: clarkson
 */

#include <od/graphics/charts/StripChart.h>

namespace od
{

	StripChart::StripChart(int left, int bottom, int width, int height) : Graphic(left, bottom, width, height)
	{
		if (width > 0)
		{
			mFifo.allocate(width);
			mFifo.clear();
		}
	}

	StripChart::~StripChart()
	{
	}

	void StripChart::setSize(int width, int height)
	{
		Graphic::setSize(width, height);
		if (width > 0)
		{
			mFifo.allocate(width);
			mFifo.clear();
		}
	}

	void StripChart::draw(FrameBuffer &fb)
	{
		Graphic::draw(fb);
		int *values = mFifo.get(mWidth);
		int *end = values + mWidth;
		int x = mWorldLeft;
		int y;

		while (values < end)
		{
			y = mWorldBottom + *(values++);
			fb.line(GRAY7, x, mWorldBottom, x, y);
			fb.pixel(WHITE, x, y);
			x++;
		}
	}

	void StripChart::addValue(float value)
	{
		int y = (int)((value - mMin) * mScale);
		mFifo.push(y);
	}

	void StripChart::setRange(float min, float max)
	{
		mMin = MIN(min, max);
		mMax = MAX(min, max);
		mScale = mHeight / (mMax - mMin);
		mFifo.clear();
	}

} /* namespace od */
