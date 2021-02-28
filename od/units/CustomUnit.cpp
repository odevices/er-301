#include <od/units/CustomUnit.h>
#include <algorithm>

namespace od
{

    CustomUnit::CustomUnit(const std::string &name, int channelCount) : Unit(name, channelCount)
    {
    }

    CustomUnit::~CustomUnit()
    {
        for (Unit *unit : mUnits)
        {
            Profiler::remove(&(unit->mExecutionTimer));
            unit->release();
        }
    }

    void CustomUnit::insertUnit(Unit *unit, int i)
    {
        if (unit == 0 || i < 0)
            return;
        unit->attach();
        Profiler::add(&unit->mExecutionTimer, unit->mName.c_str(), false);
        if (i > (int)mUnits.size())
        {
            mUnits.push_back(unit);
        }
        else
        {
            mUnits.insert(mUnits.begin() + i, unit);
        }
    }

    void CustomUnit::appendUnit(Unit *unit)
    {
        if (unit == 0)
            return;
        unit->attach();
        Profiler::add(&unit->mExecutionTimer, unit->mName.c_str(), false);
        mUnits.push_back(unit);
    }

    Unit *CustomUnit::getUnit(int i)
    {
        if (i < 0 || i >= (int)mUnits.size())
            return 0;
        return mUnits.at(i);
    }

    void CustomUnit::clearUnits()
    {
        for (Unit *unit : mUnits)
        {
            Profiler::remove(&(unit->mExecutionTimer));
            unit->release();
        }
        mUnits.clear();
    }

    void CustomUnit::removeUnit(int i)
    {
        Unit *unit = getUnit(i);
        if (unit)
        {
            Profiler::remove(&(unit->mExecutionTimer));
            mUnits.erase(mUnits.begin() + i);
            unit->release();
        }
    }

    void CustomUnit::removeUnit(Unit *unit)
    {
        std::vector<Unit *>::iterator i = std::find(mUnits.begin(), mUnits.end(),
                                                    unit);
        if (i != mUnits.end())
        {
            Profiler::remove(&(unit->mExecutionTimer));
            mUnits.erase(i);
            unit->release();
        }
    }

    int CustomUnit::getUnitPosition(Unit *unit)
    {
        std::vector<Unit *>::iterator i = std::find(mUnits.begin(), mUnits.end(),
                                                    unit);
        if (i != mUnits.end())
        {
            return (int)(i - mUnits.begin());
        }
        else
        {
            return -1;
        }
    }

    void CustomUnit::mute()
    {
        mMuted = true;
    }

    void CustomUnit::unmute()
    {
        mMuted = false;
    }

    void CustomUnit::lock()
    {
        mMutex.enter();
        disconnectUnits();
    }

    void CustomUnit::unlock()
    {
        connectUnits();
        mMutex.leave();
    }

} /* namespace od */
