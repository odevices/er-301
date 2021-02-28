#include <od/units/EffectUnit.h>
#include <hal/ops.h>

namespace od
{

  EffectUnit::EffectUnit(const std::string &name, int channelCount) : CustomUnit(name, channelCount)
  {
    for (int i = 0; i < channelCount; i++)
    {
      Monitor *o = new Monitor();
      o->attach();
      addInput(i, o, "In");
      mInputMonitors.push_back(o);
    }

    for (int i = 0; i < channelCount; i++)
    {
      Monitor *o = new Monitor();
      o->attach();
      setOutput(i, o, "Out");
      mOutputMonitors.push_back(o);
    }

    connectUnits();
  }

  EffectUnit::~EffectUnit()
  {
    disconnectUnits();

    for (Monitor *o : mInputMonitors)
    {
      o->release();
    }

    for (Monitor *o : mOutputMonitors)
    {
      o->release();
    }
  }

  void EffectUnit::connectUnits()
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
        else
        {
          // No active previous unit to connect, so connect the input monitors.
          // (connect outlets of input monitors to the inlets of the unit)
          int n = MIN(u1->getInputCount(), (int)mInputMonitors.size());
          for (int i = 0; i < n; i++)
          {
            Outlet *outlet = mInputMonitors[i]->getOutput(0);
            std::vector<Inlet *> &inlets = u1->getInputs(i);
            for (Inlet *inlet : inlets)
            {
              inlet->connect(outlet);
            }
          }
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
    else
    {
      // no units or all units bypassed, so connect input monitors to output monitors
      int n = MIN((int)mInputMonitors.size(), (int)mOutputMonitors.size());
      for (int i = 0; i < n; i++)
      {
        Outlet *outlet = mInputMonitors[i]->getOutput(0);
        Inlet *inlet = mOutputMonitors[i]->getInput(0);
        inlet->connect(outlet);
      }
    }
  }

  void EffectUnit::disconnectUnits()
  {
    // disconnect all units
    for (Unit *unit : mUnits)
    {
      unit->disconnect();
    }

    //for (Monitor * o : mInputMonitors) {
    //  o->getOutput(0)->disconnect();
    //}

    for (Monitor *o : mOutputMonitors)
    {
      o->getInput(0)->disconnect();
    }
  }

  void EffectUnit::process()
  {
    mMutex.enter();
    if (mEnabled)
    {
      Unit::process();
    }

    for (Monitor *o : mInputMonitors)
    {
      o->process();
    }
    for (Unit *unit : mUnits)
    {
      if (unit->mEnabled)
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

} /* namespace od */
