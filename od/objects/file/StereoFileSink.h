#pragma once

#include <od/objects/file/FileSink.h>

namespace od
{

	class StereoFileSink : public FileSink
	{
	public:
		StereoFileSink();
		virtual ~StereoFileSink();

#ifndef SWIGLUA
		virtual void process();
		Inlet mLeftInput{"Left In"};
		Inlet mRightInput{"Right In"};
#endif
	};

} /* namespace od */
