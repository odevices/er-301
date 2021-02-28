#pragma once

#include <od/extras/ReferenceCounted.h>
#include <od/graphics/Graphic.h>
#include <od/graphics/Cursor.h>

namespace od
{

	class GraphicContext : public ReferenceCounted
	{
	public:
		GraphicContext();
		virtual ~GraphicContext();

#ifndef SWIGLUA
		void draw(FrameBuffer &fb);
#endif

		void setGraphic(Graphic *graphic);
		void setOverlayGraphic(Graphic *graphic);
		void setCursorController(Graphic *graphic);
		void slideUpTo(Graphic *graphic, int speed);
		void slideDownTo(Graphic *graphic, int speed);
		void clear();

		void notifyHidden();

		void setCursorPosition(int x, int y)
		{
			mCursor.setPosition(x, y);
		}

		void hideCursor()
		{
			mShowCursor = false;
		}

		void showCursor()
		{
			mShowCursor = true;
		}

	private:
		Graphic *mpGraphic = 0;
		Graphic *mpOverlayGraphic = 0;

		// sliding
		Graphic *mpOldGraphic = 0;
		int mSlidingStep = 0;
		int mSlidingPosition = 0;
		int mSlidingOldPosition = 0;

		// cursor state
		Graphic *mpCursorController = 0;
		Cursor mCursor;
		bool mShowCursor = true;
	};

} /* namespace od */
