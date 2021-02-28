/*
 * RecordingGraphic.h
 *
 *  Created on: 22 Apr 2017
 *      Author: clarkson
 */

#ifndef APP_GRAPHICS_RECORDINGGRAPHIC_H_
#define APP_GRAPHICS_RECORDINGGRAPHIC_H_

#include <od/graphics/Graphic.h>
#include <od/objects/file/FileSinkThread.h>

namespace od
{

	class RecordingGraphic : public Graphic
	{
	public:
		RecordingGraphic(FileSinkThread &thread);
		virtual ~RecordingGraphic();
#ifndef SWIGLUA
		virtual void draw(FrameBuffer &fb);
#endif

		void notifyPaused(bool value)
		{
			mIsPaused = value;
		}

		void notifyFailed(bool value)
		{
			mIsFailed = value;
		}

		void toggleDisplay()
		{
			mFocusDuration = !mFocusDuration;
		}

		void reset()
		{
			mIsPaused = false;
			mIsFailed = false;
			mFocusDuration = true;
		}

	private:
		FileSinkThread &mThread;
		std::string mDurationText;
		std::string mRemainingText;
		float mLastDuration = -1.0f;
		bool mIsPaused = false;
		bool mIsFailed = false;
		int mFlashCount = 0;
		bool mFocusDuration = true;
	};

} /* namespace od */

#endif /* APP_GRAPHICS_RECORDINGGRAPHIC_H_ */
