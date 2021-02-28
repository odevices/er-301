#pragma once

#include <od/objects/file/FileSink.h>

namespace od
{

	class MonoFileSink : public FileSink
	{
	public:
		MonoFileSink();
		virtual ~MonoFileSink();

#ifndef SWIGLUA
		virtual void process();
		Inlet mInput{"In"};
#endif
	};

} /* namespace od */
