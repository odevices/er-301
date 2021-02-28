/*
 * TextEditor.h
 *
 *  Created on: 28 Mar 2017
 *      Author: clarkson
 */

#ifndef APP_GRAPHICS_TEXTEDITOR_H_
#define APP_GRAPHICS_TEXTEDITOR_H_

#include <od/graphics/Graphic.h>

namespace od
{

	class TextEditor : public Graphic
	{
	public:
		TextEditor(int left, int bottom, int width, int height, int textsize = 10);
		virtual ~TextEditor();

#ifndef SWIGLUA
		virtual void draw(FrameBuffer &fb);
#endif
		void setText(const std::string &text);
		const std::string &getText()
		{
			return mText;
		}
		void insertTextAtCaret(const std::string &text);
		void setCaretPosition(int position);
		void moveCaretToBeginning();
		void moveCaretToEnding();
		bool moveCaretLeft();
		bool moveCaretRight();
		bool doBackspace();
		bool doDelete();

		bool encoder(int change, bool shifted, int threshold);

	protected:
		std::string mText;
		int mTextWidth, mTextHeight;
		int mTextSize = 10;
		int mCaretPosition = 0;
		int mCaretLeft = 0;
		int mEncoderSum;
		bool mCalculateCaret = true;
	};

} /* namespace od */

#endif /* APP_GRAPHICS_TEXTEDITOR_H_ */
