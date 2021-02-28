/*
 * PositionDisplay.cpp
 *
 *  Created on: Oct 2, 2016
 *      Author: clarkson
 */

#include <od/graphics/sampling/PositionDisplay.h>
#include <stdio.h>

namespace od
{

	PositionDisplay::PositionDisplay(int left, int bottom, int width, int height,
																	 int textSize) : Graphic(left, bottom, width, height), mTextSize(textSize)
	{
		mText = "00:00.000";
		getTextSize(mText.c_str(), &mTextWidth, &mTextHeight, mTextSize);
	}

	PositionDisplay::~PositionDisplay()
	{
		if (mpHead)
		{
			mpHead->release();
			mpHead = 0;
		}
	}

	void PositionDisplay::setHead(Head *head)
	{
		if (mpHead)
		{
			mpHead->release();
		}
		mpHead = head;
		if (mpHead)
		{
			mpHead->attach();
		}
	}

	void PositionDisplay::draw(FrameBuffer &fb)
	{
		Graphic::draw(fb);

		if (mpHead)
		{
			uint32_t pos = mpHead->getPosition();
			if (pos != mLastPosition)
			{
				mLastPosition = pos;
				float totalSecs = ((double)pos) / (double)mpHead->getSampleRate();
				int mins = totalSecs / 60;
				int secs = totalSecs - mins * 60;
				int ms = 1000 * (totalSecs - mins * 60 - secs);
				char tmp[32];
				snprintf(tmp, sizeof(tmp), "%02d:%02d.%03d", mins, secs, ms);
				mText = tmp;
				//getTextSize(tmp, &mTextWidth, &mTextHeight, mTextSize);
			}
		}

		switch (mJustification)
		{
		case justifyLeft:
			fb.text(mForeground, mWorldLeft, mWorldBottom, mText.c_str(),
							mTextSize);
			break;
		case justifyCenter:
			fb.text(mForeground, mWorldLeft + (mWidth - mTextWidth) / 2,
							mWorldBottom, mText.c_str(), mTextSize);
			break;
		case justifyRight:
			fb.text(mForeground, mWorldLeft + mWidth - mTextWidth, mWorldBottom,
							mText.c_str(), mTextSize);
			break;
		}
	}

} /* namespace od */
