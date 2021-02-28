#pragma once

#include <core/units/BandedUnit.h>
#include <od/objects/measurement/Monitor.h>
#include <hal/concurrency/Mutex.h>

namespace od
{

    class ChainBand : public ReferenceCounted
    {
    public:
        ChainBand(const std::string &name, int channelCount, BandedUnit *pUnit);
        virtual ~ChainBand();

#ifndef SWIGLUA
        void process();
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

        void mute();
        void unmute();
        bool isMuted()
        {
            return mMuted;
        }

        Outlet *getOutput(int i);

    protected:
        BandedUnit *mpParentUnit;
        // in processing order
        std::vector<Unit *> mUnits;
        std::vector<Monitor *> mOutputMonitors;
        Mutex mMutex;
        bool mMuted = false;

        void connectUnits();
        void disconnectUnits();
    };

} /* namespace od */
