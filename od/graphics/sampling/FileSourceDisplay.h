/*
 * FileSourceDisplay.h
 *
 *  Created on: 6 May 2017
 *      Author: clarkson
 */

#ifndef APP_GRAPHICS_FILESOURCEDISPLAY_H_
#define APP_GRAPHICS_FILESOURCEDISPLAY_H_

#include <od/graphics/Graphic.h>
#include <od/graphics/text/Label.h>
#include <od/objects/file/FileSource.h>

namespace od
{

	class FileSourceDisplay : public Graphic
	{
	public:
		FileSourceDisplay(int left, int bottom, int width, int height);
		virtual ~FileSourceDisplay();

#ifndef SWIGLUA
		virtual void draw(FrameBuffer &fb);
#endif

		void setFileSource(FileSource *source);
		void setDisplayName(const std::string &name);

	protected:
		FileSource *mpSource = 0;
		uint32_t mLastPosition = 0;
		uint32_t mLastDuration = 0;
		Label mPositionLabel{"0:00:00.000", 10};
		Label mDurationLabel{"0:00:00.000", 10};
		Label mFilenameLabel{"None", 10};
		Label mStatusLabel{"Unknown", 10};
	};

} /* namespace od */

#endif /* APP_GRAPHICS_FILESOURCEDISPLAY_H_ */
