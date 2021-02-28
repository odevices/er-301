/*
 * PositionDisplay.h
 *
 *  Created on: Oct 2, 2016
 *      Author: clarkson
 */

#ifndef APP_GRAPHICS_POSITIONDISPLAY_H_
#define APP_GRAPHICS_POSITIONDISPLAY_H_

#include <od/objects/heads/Head.h>
#include <od/graphics/Graphic.h>

namespace od
{

	class PositionDisplay : public Graphic
	{
	public:
		PositionDisplay(int left, int bottom, int width, int height, int textSize);
		virtual ~PositionDisplay();

#ifndef SWIGLUA
		virtual void draw(FrameBuffer &fb);
#endif

		void setHead(Head *head);

		void setJustification(Justification mode)
		{
			mJustification = mode;
		}

	protected:
		Head *mpHead = 0;
		uint32_t mLastPosition = 0;

		int mTextWidth, mTextHeight;
		std::string mText;
		int mTextSize;
		Justification mJustification = justifyCenter;
	};

} /* namespace od */

#endif /* APP_GRAPHICS_POSITIONDISPLAY_H_ */
