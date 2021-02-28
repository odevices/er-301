/*
 * RecordingGraphic.cpp
 *
 *  Created on: 22 Apr 2017
 *      Author: clarkson
 */

#include <od/graphics/recording/RecordingGraphic.h>
#include <stdio.h>

namespace od
{

	RecordingGraphic::RecordingGraphic(FileSinkThread &thread) : Graphic(0, 0, 128, 64), mThread(thread)
	{
	}

	RecordingGraphic::~RecordingGraphic()
	{
	}

	void RecordingGraphic::draw(FrameBuffer &fb)
	{
		Graphic::draw(fb);
		float used = mThread.percentUsed();
		float maxUsed = mThread.percentMaximumUsed();
		int w1 = mWidth * maxUsed;
		w1 = MIN(MAX(w1, 0), mWidth);
		int w2 = mWidth * used;
		w2 = MIN(MAX(w2, 0), mWidth);

		fb.hline(WHITE, mWorldLeft, mWorldLeft + w2 - 1,
						 mWorldBottom + mHeight - 1);
		fb.pixel(WHITE, mWorldLeft + w1, mWorldBottom + mHeight - 1);

		if (mIsFailed)
		{
			const char *text = "Recording failed.";
			int height, width;
			getTextSize(text, &width, &height, 10);
			fb.text(WHITE, mWorldLeft + (mWidth - width) / 2, GRID5_LINE1 + 2, text, 10);
		}
		else if (mIsPaused)
		{
			const char *text = "Paused...";
			int height, width;
			getTextSize(text, &width, &height, 10);
			fb.text(WHITE, mWorldLeft + (mWidth - width) / 2, GRID5_LINE1 + 2, text, 10);
		}
		else
		{
			const std::string &text = mThread.status();
			int height, width;
			getTextSize(text.c_str(), &width, &height, 10);
			fb.text(WHITE, mWorldLeft + (mWidth - width) / 2, GRID5_LINE1 + 2, text.c_str(), 10);
		}
		fb.hline(WHITE, mWorldLeft, mWorldLeft + mWidth, GRID5_LINE1 - 4);

		////////////
		float duration = mThread.duration();
		if (mLastDuration != duration)
		{
			char tmp[32];
			mLastDuration = duration;
			int hours = (int)(duration / 3600.0f);
			duration -= hours * 3600;
			int mins = (int)(duration / 60.0f);
			duration -= mins * 60;
			int secs = (int)duration;
			snprintf(tmp, sizeof(tmp), "%d:%02d:%02d", hours, mins, secs);
			mDurationText = tmp;
			float remaining = mThread.remaining();
			hours = (int)(remaining / 3600.0f);
			remaining -= hours * 3600;
			mins = (int)(remaining / 60.0f);
			remaining -= mins * 60;
			secs = (int)remaining;
			snprintf(tmp, sizeof(tmp), "%d:%02d:%02d", hours, mins, secs);
			mRemainingText = tmp;
		}

		if (mFocusDuration)
		{
			fb.text(WHITE, mWorldLeft + 20, GRID4_LINE2 - 2, mDurationText.c_str(),
							16);
			fb.text(WHITE, mWorldLeft + 3, GRID5_LINE4 - 1, mRemainingText.c_str(),
							10);
			fb.text(WHITE, mWorldLeft, GRID5_LINE5 + 2, "remaining", 10);
		}
		else
		{
			fb.text(WHITE, mWorldLeft + 20, GRID4_LINE2 - 2, mRemainingText.c_str(),
							16);
			fb.text(WHITE, mWorldLeft + 3, GRID5_LINE4 - 1, mDurationText.c_str(),
							10);
			fb.text(WHITE, mWorldLeft, GRID5_LINE5 + 2, "recorded", 10);
		}

		if (mIsFailed)
		{
		}
		else
		{
			if (mIsPaused && sTween < 0.5f)
			{
				fb.fill(WHITE, mWorldLeft + 42 + 12, mWorldBottom + 3,
								mWorldLeft + 42 + 18, mWorldBottom + 20);
				fb.fill(WHITE, mWorldLeft + 42 + 24, mWorldBottom + 3,
								mWorldLeft + 42 + 30, mWorldBottom + 20);
			}
			else
			{
				fb.box(WHITE, mWorldLeft + 42 + 12, mWorldBottom + 3,
							 mWorldLeft + 42 + 18, mWorldBottom + 20);
				fb.box(WHITE, mWorldLeft + 42 + 24, mWorldBottom + 3,
							 mWorldLeft + 42 + 30, mWorldBottom + 20);
			}

			fb.box(WHITE, mWorldLeft + 84 + 12, mWorldBottom + 3, mWorldLeft + 84 + 30,
						 mWorldBottom + 20);
		}
	}

} /* namespace od */
