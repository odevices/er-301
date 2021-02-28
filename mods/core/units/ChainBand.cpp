#include <core/units/ChainBand.h>
#include <hal/ops.h>
#include <algorithm>

namespace od
{

  ChainBand::ChainBand(const std::string &name, int channelCount,
                       BandedUnit *pUnit) : mpParentUnit(pUnit)
  {
    for (int i = 0; i < channelCount; i++)
    {
      Monitor *o = new Monitor();
      o->attach();
      mOutputMonitors.push_back(o);
    }

    connectUnits();
  }

  ChainBand::~ChainBand()
  {
    disconnectUnits();
    clearUnits();

    for (Monitor *o : mOutputMonitors)
    {
      o->release();
    }
  }

  void ChainBand::insertUnit(Unit *unit, int i)
  {
    if (unit == 0 || i < 0)
      return;
    unit->attach();
    if (i > (int)mUnits.size())
    {
      mUnits.push_back(unit);
    }
    else
    {
      mUnits.insert(mUnits.begin() + i, unit);
    }
  }

  void ChainBand::appendUnit(Unit *unit)
  {
    if (unit == 0)
      return;
    unit->attach();
    mUnits.push_back(unit);
  }

  Unit *ChainBand::getUnit(int i)
  {
    if (i < 0 || i >= (int)mUnits.size())
      return 0;
    return mUnits.at(i);
  }

  void ChainBand::clearUnits()
  {
    for (Unit *unit : mUnits)
    {
      unit->release();
    }
    mUnits.clear();
  }

  void ChainBand::removeUnit(int i)
  {
    Unit *unit = getUnit(i);
    if (unit)
    {
      mUnits.erase(mUnits.begin() + i);
      unit->release();
    }
  }

  void ChainBand::removeUnit(Unit *unit)
  {
    std::vector<Unit *>::iterator i = std::find(mUnits.begin(), mUnits.end(),
                                                unit);
    if (i != mUnits.end())
    {
      mUnits.erase(i);
      unit->release();
    }
  }

  int ChainBand::getUnitPosition(Unit *unit)
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

  void ChainBand::connectUnits()
  {
    Unit *u0 = 0;

    if (mUnits.size() > 0)
    {

      // connect units in serial
      for (Unit *u1 : mUnits)
      {
        if (u0)
        {
          Unit::connect(u0, u1);
        }
        if (!u1->getBypass())
        {
          u0 = u1;
        }
      }

      // connect outlets of input monitors to the inlets of first unit
      Unit *first = *mUnits.begin();
      int n = MIN(first->getInputCount(), mpParentUnit->getInputCount());
      for (int i = 0; i < n; i++)
      {
        Outlet *outlet = mpParentUnit->mInputMonitors[i]->getOutput(0);
        std::vector<Inlet *> &inlets = first->getInputs(i);
        for (Inlet *inlet : inlets)
        {
          inlet->connect(outlet);
        }
      }
    }

    Unit *last = u0;
    if (last)
    {
      // connect inlets of output monitors to the outlets of last unit
      int n = MIN(last->getOutputCount(), (int)mOutputMonitors.size());
      for (int i = 0; i < n; i++)
      {
        Outlet *outlet = last->getOutput(i);
        if (outlet)
        {
          Inlet *inlet = mOutputMonitors[i]->getInput(0);
          inlet->connect(outlet);
        }
      }
    }
    else
    {
      // no units or all units bypassed, so connect input monitors to output monitors
      int n = MIN(mpParentUnit->getInputCount(), (int)mOutputMonitors.size());
      for (int i = 0; i < n; i++)
      {
        Outlet *outlet = mpParentUnit->mInputMonitors[i]->getOutput(0);
        Inlet *inlet = mOutputMonitors[i]->getInput(0);
        inlet->connect(outlet);
      }
    }
  }

  void ChainBand::disconnectUnits()
  {
    // disconnect all units
    for (Unit *unit : mUnits)
    {
      unit->disconnect();
    }

    for (Monitor *o : mOutputMonitors)
    {
      o->getInput(0)->disconnect();
    }
  }

  void ChainBand::process()
  {
    mMutex.enter();
    for (Unit *unit : mUnits)
    {
      if (unit->isEnabled())
      {
        unit->process();
      }
    }
    for (Monitor *o : mOutputMonitors)
    {
      o->process();
    }
    mMutex.leave();
  }

  void ChainBand::mute()
  {
    if (!mMuted)
    {
      mMuted = true;
      //mMutex.enter();
      // TODO: disconnect outputs
      //mMutex.leave();
    }
  }

  void ChainBand::unmute()
  {
    if (mMuted)
    {
      mMuted = false;
      //mMutex.enter();
      // TODO: reconnect outputs
      //mMutex.leave();
    }
  }

  void ChainBand::lock()
  {
    mMutex.enter();
    disconnectUnits();
  }

  void ChainBand::unlock()
  {
    connectUnits();
    mMutex.leave();
  }

  Outlet *ChainBand::getOutput(int i)
  {
    if (i >= 0 && i < (int)mOutputMonitors.size())
    {
      Monitor *o = mOutputMonitors[i];
      return o->getOutput(0);
    }
    else
    {
      return 0;
    }
  }

} /* namespace od */
