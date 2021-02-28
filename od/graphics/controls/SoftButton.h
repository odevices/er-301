/*
 * SoftButton.h
 *
 *  Created on: 24 Mar 2017
 *      Author: clarkson
 */

#ifndef APP_GRAPHICS_SOFTBUTTON_H_
#define APP_GRAPHICS_SOFTBUTTON_H_

#include <od/graphics/Graphic.h>
#include <string>

namespace od
{

	class SoftButton : public Graphic
	{
	public:
		SoftButton(const std::string &text, int column = 1);
		virtual ~SoftButton();

		void setText(const std::string &text);
		void setNumber(float x);

		const std::string &getText()
		{
			return mText;
		}

		void setLeftBorder(int width)
		{
			mLeftBorder = width;
		}

		void setRightBorder(int width)
		{
			mRightBorder = width;
		}

		void setTopBorder(int width)
		{
			mTopBorder = width;
		}

		void setInvert(bool value)
		{
			mInvert = value;
		}

		void setCenter(int x);

#ifndef SWIGLUA
		virtual void draw(FrameBuffer &fb);
#endif

	protected:
		std::string mText;
		std::string mDisplayText;
		int mTextWidth, mTextHeight;
		int mLeftBorder = 0;
		int mRightBorder = 0;
		int mTopBorder = 0;
		bool mInvert = false;
	};

} /* namespace od */

#endif /* APP_GRAPHICS_SOFTBUTTON_H_ */
