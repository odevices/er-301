#include <od/tasks/UnitChain.h>
#include <od/extras/LookupTables.h>
#include <hal/concurrency/Thread.h>
#include <hal/audio.h>
#include <algorithm>

namespace od
{

    UnitChain::UnitChain(const std::string &name, int channelCount) : Task(name), mChannelCount(channelCount)
    {
        own(mLeftOutput);
        own(mRightOutput);
        mFade.setLength(25);
        mFade.reset(0.0f, 1.0f);
    }

    UnitChain::~UnitChain()
    {
        disconnectInternals();
        disconnectOutputs();
        clearInputs();
        clearUnits();
    }

    void UnitChain::insertUnit(Unit *unit, int i)
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

    void UnitChain::appendUnit(Unit *unit)
    {
        if (unit == 0)
            return;
        unit->attach();
        Profiler::add(&unit->mExecutionTimer, unit->mName.c_str(), false);
        mUnits.push_back(unit);
    }

    Unit *UnitChain::getUnit(int i)
    {
        if (i < 0 || i >= (int)mUnits.size())
            return 0;
        return mUnits.at(i);
    }

    void UnitChain::clearUnits()
    {
        for (Unit *unit : mUnits)
        {
            Profiler::remove(&(unit->mExecutionTimer));
            unit->release();
        }
        mUnits.clear();
    }

    void UnitChain::removeUnit(int i)
    {
        Unit *unit = getUnit(i);
        if (unit)
        {
            Profiler::remove(&(unit->mExecutionTimer));
            mUnits.erase(mUnits.begin() + i);
            unit->release();
        }
    }

    void UnitChain::removeUnit(Unit *unit)
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

    int UnitChain::getUnitPosition(Unit *unit)
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

    void UnitChain::connectInternals()
    {
        Unit *lastActiveUnit = 0;

        // Sequentially connect units
        for (Unit *unit : mUnits)
        {
            if (lastActiveUnit)
            {
                // Connect the last active unit to this unit.
                Unit::connect(lastActiveUnit, unit);
            }
            else
            {
                // No last active unit, so connect the sources to this unit.
                if (mpLeftSource)
                {
                    std::vector<Inlet *> &inlets = unit->getInputs(0);
                    for (Inlet *inlet : inlets)
                    {
                        inlet->connect(mpLeftSource);
                    }
                }

                if (mpRightSource && mChannelCount > 1)
                {
                    std::vector<Inlet *> &inlets = unit->getInputs(1);
                    for (Inlet *inlet : inlets)
                    {
                        inlet->connect(mpRightSource);
                    }
                }
            }
            if (!unit->getBypass())
            {
                lastActiveUnit = unit;
            }
        }

        // Connect the last active unit to the outputs.
        if (lastActiveUnit)
        {
            Outlet *outlet1 = lastActiveUnit->getOutput(0);
            if (outlet1)
            {
                mLeftOutput.mInlet.connect(outlet1);
            }

            if (mChannelCount > 1)
            {
                Outlet *outlet2 = lastActiveUnit->getOutput(1);
                if (outlet2)
                {
                    mRightOutput.mInlet.connect(outlet2);
                }
            }
        }
        else
        {
            // no (active) units, so connect sources to outputs
            if (mpLeftSource)
            {
                mLeftOutput.mInlet.connect(mpLeftSource);
            }

            if (mChannelCount > 1 && mpRightSource)
            {
                mRightOutput.mInlet.connect(mpRightSource);
            }
        }
    }

    void UnitChain::disconnectInternals()
    {
        // disconnect all units
        for (Unit *unit : mUnits)
        {
            unit->disconnect();
        }

        mLeftOutput.mInlet.disconnect();
        mRightOutput.mInlet.disconnect();
    }

    void UnitChain::process(float *inputs, float *outputs)
    {
        mMutex.enter();
        for (Unit *unit : mUnits)
        {
            if (unit->mEnabled)
            {
                unit->mExecutionTimer.start();
                unit->process();
                unit->mExecutionTimer.stop();
            }
        }
        if (mMuted)
        {
            // do nothing
        }
        else if (mFade.notFinished())
        {
            // we are in the middle fading to a mute
            float w0 = mFade.mValue;
            float w1 = mFade.step();
            float a = w1 - w0;
            mLeftOutput.fade(a, w0);
            if (mChannelCount > 1)
            {
                mRightOutput.fade(a, w0);
            }
        }
        else
        {
            // normal operation
            mLeftOutput.copyInputToOutput();
            if (mChannelCount > 1)
            {
                mRightOutput.copyInputToOutput();
            }
        }

        mMutex.leave();
    }

    void UnitChain::setInput(int i, Outlet *outlet)
    {
        if (i == 0)
        {
            if (mpLeftSource)
            {
                mpLeftSource->release();
            }
            mpLeftSource = outlet;
            if (mpLeftSource)
            {
                mpLeftSource->attach();
            }
        }
        else if (i == 1 && mChannelCount > 1)
        {
            if (mpRightSource)
            {
                mpRightSource->release();
            }
            mpRightSource = outlet;
            if (mpRightSource)
            {
                mpRightSource->attach();
            }
        }
    }

    void UnitChain::clearInputs()
    {
        if (mpLeftSource)
        {
            mpLeftSource->release();
            mpLeftSource = 0;
        }
        if (mpRightSource)
        {
            mpRightSource->release();
            mpRightSource = 0;
        }
    }

    Outlet *UnitChain::getOutput(int i)
    {
        if (i < mChannelCount)
        {
            if (i == 0)
            {
                return &mLeftOutput.mOutlet;
            }
            else if (i == 1)
            {
                return &mRightOutput.mOutlet;
            }
        }
        return 0;
    }

    void UnitChain::connectOutput(int i, Inlet *inlet)
    {
        if (i < mChannelCount)
        {
            if (inlet)
            {
                if (i == 0)
                {
                    inlet->connect(&mLeftOutput.mOutlet);
                }
                else if (i == 1 && mChannelCount > 1)
                {
                    inlet->connect(&mRightOutput.mOutlet);
                }
            }
        }
    }

    void UnitChain::disconnectOutputs()
    {
        mLeftOutput.mOutlet.disconnect();
        if (mChannelCount > 1)
        {
            mRightOutput.mOutlet.disconnect();
        }
    }

    void UnitChain::mute()
    {
        if (!mMuted)
        {
            mFade.reset(0.0f);
            while (mActive && Audio_running() && mFade.notFinished())
            {
                Thread::yield();
            }
            mLeftOutput.mute();
            mRightOutput.mute();
            mMuted = true;
        }
    }

    void UnitChain::unmute()
    {
        if (mMuted)
        {
            mLeftOutput.unmute();
            mRightOutput.unmute();
            mMuted = false;
            mFade.reset(1.0f);
        }
    }

    void UnitChain::lock()
    {
        mMutex.enter();
        disconnectInternals();
    }

    void UnitChain::unlock()
    {
        connectInternals();
        mMutex.leave();
    }

} /* namespace od */
