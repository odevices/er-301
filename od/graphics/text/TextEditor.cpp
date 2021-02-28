/*
 * TextEditor.cpp
 *
 *  Created on: 28 Mar 2017
 *      Author: clarkson
 */

#include <od/graphics/text/TextEditor.h>

namespace od
{

	TextEditor::TextEditor(int left, int bottom, int width, int height,
												 int textsize) : Graphic(left, bottom, width, height), mTextSize(textsize)
	{
		mCursorState.orientation = CursorOrientation::cursorUp;
	}

	TextEditor::~TextEditor()
	{
	}

#define MARGIN 3

	void TextEditor::draw(FrameBuffer &fb)
	{

		if (mCalculateCaret)
		{
			mCalculateCaret = false;
			resetTween();
			mCaretLeft = getCaretLocation(mText.c_str(), mTextSize, mCaretPosition);
		}

		int x = mWorldLeft + MARGIN;
		if (2 * MARGIN + mCaretLeft > mWidth)
		{
			x -= mCaretLeft + 2 * MARGIN - mWidth;
		}

		Graphic::draw(fb);

		fb.text(mForeground, x,
						mWorldBottom + (mHeight - mTextHeight) / 2, mText.c_str(),
						mTextSize, ALIGN_BASE);

		Color color;
		if (fb.mIsMonoChrome)
		{
			if (sTween < 0.5f)
			{
				color = WHITE;
			}
			else
			{
				color = BLACK;
			}
		}
		else
		{
			color = sTween * WHITE;
		}
		fb.vline(color, x + mCaretLeft, mWorldBottom,
						 mWorldBottom + mHeight - 1);
		mCursorState.x = x + mCaretLeft;
		mCursorState.y = mWorldBottom - CURSOR_HEIGHT;
	}

	void TextEditor::setText(const std::string &text)
	{
		mText = text;
		getTextSize(mText.c_str(), &mTextWidth, &mTextHeight, mTextSize);
		mCaretPosition = (int)text.length();
		mCalculateCaret = true;
	}

	void TextEditor::insertTextAtCaret(const std::string &text)
	{
		mText.insert(mCaretPosition, text);
		mCaretPosition += text.length();
		mCalculateCaret = true;
	}

	void TextEditor::setCaretPosition(int position)
	{
		mCaretPosition = MIN(position, (int)mText.length());
		mCalculateCaret = true;
	}

	void TextEditor::moveCaretToBeginning()
	{
		mCaretPosition = 0;
		mCalculateCaret = true;
	}

	void TextEditor::moveCaretToEnding()
	{
		mCaretPosition = (int)mText.length();
		mCalculateCaret = true;
	}

	bool TextEditor::moveCaretLeft()
	{
		if (mCaretPosition > 0)
		{
			mCaretPosition--;
			mCalculateCaret = true;
			return true;
		}
		return false;
	}

	bool TextEditor::moveCaretRight()
	{
		if (mCaretPosition < (int)mText.length())
		{
			mCaretPosition++;
			mCalculateCaret = true;
			return true;
		}
		return false;
	}

	bool TextEditor::doBackspace()
	{
		if (mCaretPosition)
		{
			mCaretPosition--;
			mText.erase(mCaretPosition, 1);
			mCalculateCaret = true;
			return true;
		}
		else
		{
			return false;
		}
	}

	bool TextEditor::doDelete()
	{
		if (mCaretPosition < (int)mText.length())
		{
			mText.erase(mCaretPosition, 1);
			mCalculateCaret = true;
			return true;
		}
		else
		{
			return false;
		}
	}

	bool TextEditor::encoder(int change, bool shifted, int threshold)
	{
		mEncoderSum += change;
		if (mEncoderSum > threshold)
		{
			mEncoderSum = 0;
			if (shifted)
			{
				moveCaretToEnding();
			}
			else
			{
				moveCaretRight();
			}
			return true;
		}
		else if (mEncoderSum < -threshold)
		{
			mEncoderSum = 0;
			if (shifted)
			{
				moveCaretToBeginning();
			}
			else
			{
				moveCaretLeft();
			}
			return true;
		}
		return false;
	}

} /* namespace od */
