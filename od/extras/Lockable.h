#pragma once
#include <hal/concurrency/Mutex.h>

namespace od
{

	class Lockable
	{
	public:
		Lockable();
		virtual ~Lockable();

#ifndef SWIGLUA
		inline void lock()
		{
			mMutex.enter();
		}

		inline void unlock()
		{
			mMutex.leave();
		}

		inline bool tryLock()
		{
			return mMutex.tryEnter();
		}
#endif
	private:
		Mutex mMutex;
	};

} /* namespace od */
