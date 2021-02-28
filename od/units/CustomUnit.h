#pragma once
#include <od/units/Unit.h>
#include <od/objects/measurement/Monitor.h>
#include <hal/concurrency/Mutex.h>

namespace od
{

	// A unit that can also masquerade as a Chain
	class CustomUnit : public Unit
	{
	public:
		CustomUnit(const std::string &name, int channelCount);
		virtual ~CustomUnit();

		// START Chain interface
		// any modifications must be bracketed by these commands
		void lock();
		void unlock();

		// unit ops
		void insertUnit(Unit *unit, int i);
		void appendUnit(Unit *unit);
		Unit *getUnit(int i);
		void removeUnit(int i);
		void removeUnit(Unit *unit);
		void clearUnits();
		int size()
		{
			return (int)mUnits.size();
		}
		int getUnitPosition(Unit *unit);

		// Warning: This mute/unmute interface does not do anything.
		void mute();
		void unmute();
		bool isMuted()
		{
			return mMuted;
		}
		// END Chain interface

	protected:
		// in processing order
		std::vector<Unit *> mUnits;
		Mutex mMutex;
		bool mMuted = false;

		virtual void connectUnits() = 0;
		virtual void disconnectUnits() = 0;
	};

} /* namespace od */
