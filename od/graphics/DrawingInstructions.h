#pragma once

#include <od/extras/ReferenceCounted.h>
#include <vector>
#include <od/graphics/FrameBuffer.h>

namespace od
{

	enum DrawingCodes
	{
		baseCode = 10000,
		lineCode,
		vlineCode,
		hlineCode,
		triangleCode,
		circleCode,
		boxCode,
		fillCode,
		roundedBoxCode,
		polyStartCode,
		vertexCode,
		polyEndCode,
		colorCode
	};

	class DrawingInstructions : public ReferenceCounted
	{
	public:
		DrawingInstructions();
		virtual ~DrawingInstructions();

#ifndef SWIGLUA
		void draw(FrameBuffer &fb, int x, int y);
#endif

		void clear()
		{
			mData.clear();
		}

		void line(int x0, int y0, int x1, int y1);
		void vline(int x, int y0, int y1);
		void hline(int x0, int x1, int y);
		void triangle(int x, int y, int angle, int height);
		void circle(int x, int y, int radius);
		void box(int left, int bottom, int width, int height);
		void fill(int left, int bottom, int width, int height);
		void roundedBox(int left, int bottom, int width, int height,
										int radiusBL, int radiusTL, int radiusTR, int radiusBR);
		void startPolyline(int x, int y, int cornerRadius);
		void vertex(int x, int y);
		void endPolyline(int x, int y);

		void color(Color color);

#ifdef BUILDOPT_TESTING
		void dump();
#endif

	private:
		std::vector<int> mData;

		Color mPenColor = WHITE;
		int mWorldLeft;
		int mWorldBottom;

		int renderLine(FrameBuffer &fb, int i);
		int renderVLine(FrameBuffer &fb, int i);
		int renderHLine(FrameBuffer &fb, int i);
		int renderTriangle(FrameBuffer &fb, int i);
		int renderCircle(FrameBuffer &fb, int i);
		int renderBox(FrameBuffer &fb, int i);
		int renderFill(FrameBuffer &fb, int i);
		int renderRoundedBox(FrameBuffer &fb, int i);
		int renderPolyline(FrameBuffer &fb, int i);
	};

} /* namespace od */
