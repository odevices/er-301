#include <od/units/SourceUnit.h>
#include <hal/ops.h>

namespace od
{

  SourceUnit::SourceUnit(const std::string &name, int channelCount) : CustomUnit(name, channelCount)
  {
    for (int i = 0; i < channelCount; i++)
    {
      Monitor *o = new Monitor();
      o->attach();
      setOutput(i, o, "Out");
      mOutputMonitors.push_back(o);
    }

    connectUnits();
  }

  SourceUnit::~SourceUnit()
  {
    disconnectUnits();

    for (Monitor *o : mOutputMonitors)
    {
      o->release();
    }
  }

  void SourceUnit::connectUnits()
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
  }

  void SourceUnit::disconnectUnits()
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

  void SourceUnit::process()
  {
    mMutex.enter();
    if (mEnabled)
    {
      Unit::process();
    }

    for (Unit *unit : mUnits)
    {
      if (unit->mEnabled)
      {
        unit->mExecutionTimer.start();
        unit->process();
        unit->mExecutionTimer.stop();
      }
    }
    for (Monitor *o : mOutputMonitors)
    {
      o->process();
    }
    mMutex.leave();
  }

} /* namespace od */
