#pragma once

#include <od/graphics/FrameBuffer.h>

namespace od
{

#define CURSOR_HEIGHT 5

	enum CursorOrientation
	{
		cursorDown,
		cursorUp,
		cursorLeft,
		cursorRight
	};

	struct CursorState
	{
		void copyAttributes(CursorState &cs);

		CursorOrientation orientation = cursorDown;
		Color color = WHITE;
		int x = -10;
		int y = -10;
		bool active = false;
		bool show = true;
	};

	class Cursor
	{
	public:
		Cursor();
		virtual ~Cursor();
		void draw(CursorState &target, FrameBuffer &fb);
		void setPosition(int x, int y);

	private:
		float mX = 0;
		float mY = 0;
		float mBreath = 0;
		int mBreathState = 0;
	};

} /* namespace od */
