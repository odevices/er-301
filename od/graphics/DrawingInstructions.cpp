#include <od/graphics/DrawingInstructions.h>
#include <hal/log.h>

namespace od
{

	DrawingInstructions::DrawingInstructions()
	{
	}

	DrawingInstructions::~DrawingInstructions()
	{
	}

	void DrawingInstructions::draw(FrameBuffer &fb, int x, int y)
	{
		int code, n = (int)mData.size();
		int i = 0;

		mWorldLeft = x;
		mWorldBottom = y;
		mPenColor = WHITE;

		while (i < n)
		{
			code = mData[i++];
			switch (code)
			{
			case lineCode:
				i = renderLine(fb, i);
				break;
			case hlineCode:
				i = renderHLine(fb, i);
				break;
			case vlineCode:
				i = renderVLine(fb, i);
				break;
			case triangleCode:
				i = renderTriangle(fb, i);
				break;
			case circleCode:
				i = renderCircle(fb, i);
				break;
			case boxCode:
				i = renderBox(fb, i);
				break;
			case fillCode:
				i = renderFill(fb, i);
				break;
			case roundedBoxCode:
				i = renderRoundedBox(fb, i);
				break;
			case polyStartCode:
				i = renderPolyline(fb, i);
				break;
			case colorCode:
				mPenColor = (Color)mData[i++];
				break;
			}
		}
	}

	int DrawingInstructions::renderLine(FrameBuffer &fb, int i)
	{
		int x0 = mWorldLeft + mData[i++];
		int y0 = mWorldBottom + mData[i++];
		int x1 = mWorldLeft + mData[i++];
		int y1 = mWorldBottom + mData[i++];
		fb.line(mPenColor, x0, y0, x1, y1);
		return i;
	}

	int DrawingInstructions::renderVLine(FrameBuffer &fb, int i)
	{
		int x = mWorldLeft + mData[i++];
		int y0 = mWorldBottom + mData[i++];
		int y1 = mWorldBottom + mData[i++];
		fb.vline(mPenColor, x, y0, y1);
		return i;
	}

	int DrawingInstructions::renderHLine(FrameBuffer &fb, int i)
	{
		int x0 = mWorldLeft + mData[i++];
		int x1 = mWorldLeft + mData[i++];
		int y = mWorldBottom + mData[i++];
		fb.hline(mPenColor, x0, x1, y);
		return i;
	}

	int DrawingInstructions::renderTriangle(FrameBuffer &fb, int i)
	{
		int x = mWorldLeft + mData[i++];
		int y = mWorldBottom + mData[i++];
		int angle = mData[i++];
		int height = mData[i++];
		switch (angle)
		{
		case 0:
			fb.drawRightTriangle(mPenColor, x, y, height);
			break;
		case 90:
			fb.drawUpTriangle(mPenColor, x, y, height);
			break;
		case 180:
			fb.drawLeftTriangle(mPenColor, x, y, height);
			break;
		case 270:
			fb.drawDownTriangle(mPenColor, x, y, height);
			break;
		}
		return i;
	}

	int DrawingInstructions::renderCircle(FrameBuffer &fb, int i)
	{
		int x = mWorldLeft + mData[i++];
		int y = mWorldBottom + mData[i++];
		int radius = mData[i++];
		fb.circle(mPenColor, x, y, radius);
		return i;
	}

	int DrawingInstructions::renderBox(FrameBuffer &fb, int i)
	{
		int left = mWorldLeft + mData[i++];
		int bottom = mWorldBottom + mData[i++];
		int right = left + mData[i++];
		int top = bottom + mData[i++];
		fb.box(mPenColor, left, bottom, right, top);
		return i;
	}

	int DrawingInstructions::renderFill(FrameBuffer &fb, int i)
	{
		int left = mWorldLeft + mData[i++];
		int bottom = mWorldBottom + mData[i++];
		int right = left + mData[i++];
		int top = bottom + mData[i++];
		fb.fill(mPenColor, left, bottom, right, top);
		return i;
	}

	int DrawingInstructions::renderRoundedBox(FrameBuffer &fb, int i)
	{
		int left = mWorldLeft + mData[i++];
		int bottom = mWorldBottom + mData[i++];
		int right = left + mData[i++];
		int top = bottom + mData[i++];
		int radiusBL = mData[i++];
		int radiusTL = mData[i++];
		int radiusTR = mData[i++];
		int radiusBR = mData[i++];
		// top border
		fb.line(mPenColor, left + radiusTL, top, right - radiusTR, top);
		// bottom border
		fb.line(mPenColor, left + radiusBL, bottom, right - radiusBR, bottom);
		// left border
		fb.line(mPenColor, left, bottom + radiusBL, left, top - radiusTL);
		// right border
		fb.line(mPenColor, right, bottom + radiusBR, right, top - radiusTR);

		// top-left corner
		fb.arc8(mPenColor, left + radiusTL, top - radiusTL, radiusTL,
						0b00001100);
		// top-right corner
		fb.arc8(mPenColor, right - radiusTR, top - radiusTR, radiusTR,
						0b00000011);
		// bottom-right corner
		fb.arc8(mPenColor, right - radiusBR, bottom + radiusBR, radiusBR,
						0b11000000);
		// bottom-left corner
		fb.arc8(mPenColor, left + radiusBL, bottom + radiusBL, radiusBL,
						0b00110000);
		return i;
	}

	int DrawingInstructions::renderPolyline(FrameBuffer &fb, int i)
	{
		int n = (int)mData.size();
		int code = vertexCode, x0, y0, x1, y1; //, radius;
		// read first vertex
		x1 = mWorldLeft + mData[i++];
		y1 = mWorldBottom + mData[i++];
		//radius = mData[i++];
		// radius parameter is not implemented. ignore.
		i++;
		while (code == vertexCode && i < n)
		{
			code = mData[i++];
			x0 = x1;
			y0 = y1;
			x1 = mWorldLeft + mData[i++];
			y1 = mWorldBottom + mData[i++];
			fb.line(mPenColor, x0, y0, x1, y1);
		}
		return i;
	}

	void DrawingInstructions::color(Color color)
	{
		mData.push_back(colorCode);
		mData.push_back(color);
	}

	void DrawingInstructions::line(int x0, int y0, int x1, int y1)
	{
		mData.push_back(lineCode);
		mData.push_back(x0);
		mData.push_back(y0);
		mData.push_back(x1);
		mData.push_back(y1);
	}

	void DrawingInstructions::vline(int x, int y0, int y1)
	{
		mData.push_back(vlineCode);
		mData.push_back(x);
		mData.push_back(y0);
		mData.push_back(y1);
	}

	void DrawingInstructions::hline(int x0, int x1, int y)
	{
		mData.push_back(hlineCode);
		mData.push_back(x0);
		mData.push_back(x1);
		mData.push_back(y);
	}

	void DrawingInstructions::triangle(int x, int y, int angle, int height)
	{
		mData.push_back(triangleCode);
		mData.push_back(x);
		mData.push_back(y);
		mData.push_back(angle);
		mData.push_back(height);
	}
	void DrawingInstructions::circle(int x, int y, int radius)
	{
		mData.push_back(circleCode);
		mData.push_back(x);
		mData.push_back(y);
		mData.push_back(radius);
	}

	void DrawingInstructions::box(int left, int bottom, int width, int height)
	{
		mData.push_back(boxCode);
		mData.push_back(left);
		mData.push_back(bottom);
		mData.push_back(width);
		mData.push_back(height);
	}

	void DrawingInstructions::fill(int left, int bottom, int width, int height)
	{
		mData.push_back(fillCode);
		mData.push_back(left);
		mData.push_back(bottom);
		mData.push_back(width);
		mData.push_back(height);
	}

	void DrawingInstructions::roundedBox(int left, int bottom, int width, int height,
																			 int radiusBL, int radiusTL, int radiusTR, int radiusBR)
	{
		mData.push_back(boxCode);
		mData.push_back(left);
		mData.push_back(bottom);
		mData.push_back(width);
		mData.push_back(height);
		mData.push_back(radiusBL);
		mData.push_back(radiusTL);
		mData.push_back(radiusTR);
		mData.push_back(radiusBR);
	}

	void DrawingInstructions::startPolyline(int x, int y, int cornerRadius)
	{
		mData.push_back(polyStartCode);
		mData.push_back(x);
		mData.push_back(y);
		mData.push_back(cornerRadius);
	}

	void DrawingInstructions::vertex(int x, int y)
	{
		mData.push_back(vertexCode);
		mData.push_back(x);
		mData.push_back(y);
	}

	void DrawingInstructions::endPolyline(int x, int y)
	{
		mData.push_back(polyEndCode);
		mData.push_back(x);
		mData.push_back(y);
	}

#ifdef BUILDOPT_TESTING
	void DrawingInstructions::dump()
	{
		int code, n = (int)mData.size();
		int i = 0;

		while (i < n)
		{
			code = mData[i++];
			switch (code)
			{
			case lineCode:
				logInfo("line ");
				i += 4;
				break;
			case hlineCode:
				logInfo("hline ");
				i += 3;
				break;
			case vlineCode:
				logInfo("vline ");
				i += 3;
				break;
			case triangleCode:
				logInfo("triangle ");
				i += 4;
				break;
			case circleCode:
				logInfo("circle ");
				i += 3;
				break;
			case boxCode:
				logInfo("box ");
				i += 4;
				break;
			case fillCode:
				logInfo("fill ");
				i += 4;
				break;
			case roundedBoxCode:
				logInfo("roundedBox ");
				i += 8;
				break;
			case polyStartCode:
				logInfo("polyline start ");
				i += 3;
				break;
			case vertexCode:
				i += 2;
				logInfo("vertex ");
				break;
			case polyEndCode:
				logInfo("polyline end ");
				i += 2;
				break;
			case colorCode:
				logInfo("color(%d) ", mData[i++]);
				break;
			}
		}
	}
#endif

} /* namespace od */
