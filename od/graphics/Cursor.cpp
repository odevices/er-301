#include <od/graphics/Cursor.h>
#include <hal/constants.h>
#include <math.h>

#define TWEEN_STEP (512.0f / GRAPHICS_REFRESH_RATE)
#define BREATH_STEP (7.0f / GRAPHICS_REFRESH_RATE)

namespace od
{

	void CursorState::copyAttributes(CursorState &cs)
	{
		orientation = cs.orientation;
		color = cs.color;
		x = cs.x;
		y = cs.y;
	}

	Cursor::Cursor()
	{
	}

	Cursor::~Cursor()
	{
	}

	void Cursor::draw(CursorState &target, FrameBuffer &fb)
	{
		int left, bottom, breath;
		float dx = fabs(target.x - mX);
		float dy = fabs(target.y - mY);

		// tween towards the target state
		if (dx > 1)
		{
			if (mX < target.x)
			{
				mX += MIN(dx, TWEEN_STEP);
			}
			else
			{
				mX -= MIN(dx, TWEEN_STEP);
			}
		}
		left = (int)mX;

		if (dy > 1)
		{
			if (mY < target.y)
			{
				mY += MIN(dy, TWEEN_STEP);
			}
			else
			{
				mY -= MIN(dy, TWEEN_STEP);
			}
		}
		bottom = (int)mY;

		switch (mBreathState)
		{
		case 0:
			if (mBreath < 3.5f)
			{
				mBreath += BREATH_STEP;
			}
			else
			{
				mBreathState = 1;
			}
			break;
		case 1:
			if (mBreath > 0.0f)
			{
				mBreath -= BREATH_STEP;
			}
			else
			{
				mBreathState = 0;
			}
			break;
		}

		breath = (int)mBreath;

		if (target.show)
		{
			switch (target.orientation)
			{
			case cursorDown:
				//outlineDownTriangle(fb, target.color, left, bottom - breath,
				//CURSOR_HEIGHT);
				{
					int x = left;
					int y = bottom - breath;
					fb.hline(target.color, x - 4, x + 4, y);
					y--;
					fb.pixel(target.color, x - 3, y);
					fb.pixel(target.color, x + 3, y);
					fb.clear(x - 2, y);
					fb.clear(x - 1, y);
					fb.clear(x, y);
					fb.clear(x + 1, y);
					fb.clear(x + 2, y);
					y--;
					fb.pixel(target.color, x - 2, y);
					fb.pixel(target.color, x + 2, y);
					fb.clear(x - 1, y);
					fb.clear(x, y);
					fb.clear(x + 1, y);
					y--;
					fb.pixel(target.color, x - 1, y);
					fb.pixel(target.color, x + 1, y);
					fb.clear(x, y);
					y--;
					fb.pixel(target.color, x, y);
				}
				break;
			case cursorUp:
				fb.drawUpTriangle(target.color, left, bottom + breath,
													CURSOR_HEIGHT);
				break;
			case cursorLeft:
				fb.drawLeftTriangle(target.color, left - breath, bottom,
														CURSOR_HEIGHT);
				break;
			case cursorRight:
				fb.drawRightTriangle(target.color, left + breath, bottom,
														 CURSOR_HEIGHT);
				break;
			}
		}
	}

	void Cursor::setPosition(int x, int y)
	{
		mX = x;
		mY = y;
	}

} /* namespace od */
