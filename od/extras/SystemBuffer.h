#pragma once

namespace od
{

	class SystemBuffer
	{
	public:
		SystemBuffer();
		virtual ~SystemBuffer();

		bool allocate(int n);
		void deallocate();

		char *mpData = 0;
		int mSizeInBytes = 0;
	};

} /* namespace od */
