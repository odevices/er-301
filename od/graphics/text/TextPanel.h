/*
 * Panel.h
 *
 *  Created on: 8 Apr 2017
 *      Author: clarkson
 */

#ifndef APP_GRAPHICS_TEXTPANEL_H_
#define APP_GRAPHICS_TEXTPANEL_H_

#include <od/graphics/Graphic.h>
#include <string>
#include <vector>

namespace od
{

	class TextPanel : public Graphic
	{
	public:
		TextPanel(const std::string &text, int column, float textRatio = 0.5f, int height = 64);
		virtual ~TextPanel();

#ifndef SWIGLUA
		virtual void draw(FrameBuffer &fb);
#endif
		void clear();
		void setText(const std::string &text);
		void addLine(const std::string &line);
		void setLine(int index, const std::string &line);
		int getLineCount()
		{
			return (int)mLines.size();
		}

		void imitateSectionStyle(SectionStyle style);

		void setLeftBorder(int width)
		{
			mLeftBorder = width;
		}

		void setRightBorder(int width)
		{
			mRightBorder = width;
		}

		void setTextSize(int size)
		{
			mTextSize = size;
		}

		void setMargin(int margin)
		{
			mMargin = margin;
		}

		void setJustification(Justification style)
		{
			mJustification = style;
		}

		void setTextRatio(float ratio)
		{
			mTextHeight = mHeight * ratio;
		}

		void select()
		{
			mSelected = true;
		}

		void deselect()
		{
			mSelected = false;
		}

	protected:
		std::vector<std::string> mLines;
		int mLeftBorder = 0;
		int mRightBorder = 0;
		int mTextSize = 10;
		int mMargin = 2;
		int mTextHeight = 0;
		Justification mJustification = justifyCenter;
		bool mSelected = false;
	};

} /* namespace od */

#endif /* APP_GRAPHICS_TEXTPANEL_H_ */
