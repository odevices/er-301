#pragma once

#include <od/tasks/Task.h>
#include <od/units/Unit.h>
#include <od/objects/Repeater.h>
#include <hal/concurrency/Mutex.h>
#include <od/extras/LinearRamp.h>

namespace od
{

	class UnitChain : public Task
	{
	public:
		UnitChain(const std::string &name, int channelCount);
		virtual ~UnitChain();

#ifndef SWIGLUA
		virtual void process(float *inputs, float *outputs);
#endif

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

		// output ops
		void connectOutput(int i, Inlet *inlet);
		void disconnectOutputs();
		Outlet *getOutput(int i);

		// input ops
		void setInput(int i, Outlet *outlet);
		void clearInputs();

		// muting
		void mute();
		void unmute();
		bool isMuted()
		{
			return mMuted;
		}

	private:
		Repeater mLeftOutput;
		Repeater mRightOutput;
		Outlet *mpLeftSource = 0;
		Outlet *mpRightSource = 0;
		int mChannelCount;
		std::vector<Unit *> mUnits; // in processing order
		Mutex mMutex;
		LinearRamp mFade;
		bool mMuted = false;

		void connectInternals();
		void disconnectInternals();
	};

} /* namespace od */
