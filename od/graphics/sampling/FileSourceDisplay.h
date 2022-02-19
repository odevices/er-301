#pragma once

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
    Label mPositionLabel{"", 10};
		Label mDurationLabel{"", 10};
		Label mFilenameLabel{"", 10};
		Label mStatusLabel{"", 10};
	};

} /* namespace od */
