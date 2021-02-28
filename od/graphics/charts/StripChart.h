/*
 * StripChart.h
 *
 *  Created on: 23 Aug 2016
 *      Author: clarkson
 */

#ifndef APP_GRAPHICS_STRIPCHART_H_
#define APP_GRAPHICS_STRIPCHART_H_

#include <od/graphics/Graphic.h>
#include <od/extras/Fifo.h>

namespace od
{

	class StripChart : public Graphic
	{
	public:
		StripChart(int left, int bottom, int width, int height);
		virtual ~StripChart();

#ifndef SWIGLUA
		virtual void draw(FrameBuffer &fb);
#endif

		virtual void setSize(int width, int height);

		void addValue(float value);
		void setRange(float min, float max);

	private:
		Fifo<int> mFifo;
		float mMin = 0.0f;
		float mMax = 1.0f;
		float mScale = 1.0f;
	};

} /* namespace od */

#endif /* APP_GRAPHICS_STRIPCHART_H_ */
