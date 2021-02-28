#pragma once

#include <od/graphics/Graphic.h>
#include <string>

namespace od
{

	class Label : public Graphic
	{
	public:
		Label(const std::string &text);
		Label(const std::string &text, int size);
		Label(const std::string &text, int size, int direction);
		virtual ~Label();
#ifndef SWIGLUA
		virtual void draw(FrameBuffer &fb);
#endif
		void setText(const std::string &text);
		void setNumber(float x);
		void setTextWithSize(const std::string &text, int size);
		void setTextDirection(int direction);
		void fitToText(int margin);

		void setJustification(Justification mode)
		{
			mJustification = mode;
		}

		int getTextWidth()
		{
			return mTextWidth;
		}

		int getTextHeight()
		{
			return mTextHeight;
		}

		const std::string &getText()
		{
			return mText;
		}

	protected:
		std::string mText;
		int mTextWidth, mTextHeight;
		int mTextDirection = TEXT_HORIZONTAL;
		int mTextSize = 10;
		int mMargin = 0;
		Justification mJustification = justifyCenter;
	};

} /* namespace od */
