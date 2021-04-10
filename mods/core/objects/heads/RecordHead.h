#pragma once

#include <od/objects/heads/TapeHead.h>
#include <od/extras/LinearRamp.h>

namespace od
{

	class RecordHead : public TapeHead
	{
	public:
		RecordHead();
		virtual ~RecordHead();

		void zeroBuffer();

	private:
		typedef TapeHead Base;
	};

} /* namespace od */
