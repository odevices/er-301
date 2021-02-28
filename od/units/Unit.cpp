#include <od/units/Unit.h>
#include <od/units/GraphCompiler.h>
#include <hal/log.h>
#include <hal/ops.h>
#include <sstream>
#include <algorithm>
#include <set>

#define OBJECT_TIMING_ENABLED 0

namespace od
{

  Unit::Unit(const std::string &name, int channelCount) : mName(name)
  {
    if (channelCount > 0)
    {
      mInputs.resize(channelCount);
    }

    if (channelCount < 1)
    {
      channelCount = 1;
    }
    mOutputs.reserve(channelCount);
    for (int i = 0; i < channelCount; i++)
    {
      mOutputs.push_back(0);
    }
  }

  Unit::~Unit()
  {
    for (Object *o : mObjects)
    {
#if OBJECT_TIMING_ENABLED
      Profiler::remove(&(o->mExecutionTimer));
#endif
      o->disconnectAll();
      o->release();
    }
  }

  Outlet *Unit::getInwardConnection(int i)
  {
    if (i < 0 || i >= (int)mInputs.size())
      return 0;

    if (mInputs[i].size() == 0)
      return 0;

    return mInputs[i][0]->mInwardConnection;
  }

  bool Unit::isSource()
  {
    for (std::vector<Inlet *> &inlets : mInputs)
    {
      if (inlets.size() > 0)
      {
        return false;
      }
    }
    return true;
  }

  bool Unit::addInput(int i, Object *object, const std::string &inletName)
  {
    if (i < 0 || i >= (int)mInputs.size())
      return false;

    if (object == NULL)
    {
      return false;
    }

    Inlet *port = object->getInput(inletName);
    if (port == 0)
      return false;

    port->mIndex = i;
    mInputs[i].push_back(port);
    object->mIsInput = true;
    return true;
  }

  bool Unit::setOutput(int i, Object *object, const std::string &outletName)
  {
    if (i < 0 || i >= (int)mOutputs.size())
      return false;

    if (object == NULL)
    {
      return false;
    }

    Outlet *port = object->getOutput(outletName);
    if (port == 0)
      return false;

    port->mIndex = i;
    mOutputs[i] = port;
    object->mIsOutput = true;
    return true;
  }

  bool Unit::setOutput(int i, Object *object, int outletIndex)
  {
    if (i < 0 || i >= (int)mOutputs.size())
      return false;

    if (object == NULL)
    {
      return false;
    }

    Outlet *port = object->getOutput(outletIndex);
    if (port == 0)
      return false;

    port->mIndex = i;
    mOutputs[i] = port;
    object->mIsOutput = true;
    return true;
  }

  Outlet *Unit::getOutput(int i)
  {
    if (i < 0 || i >= (int)mOutputs.size())
      return 0;

    return mOutputs.at(i);
  }

  std::vector<Inlet *> &Unit::getInputs(int i)
  {
    return mInputs.at(i);
  }

  void Unit::disconnect()
  {
    int i, n;
    n = mInputs.size();
    for (i = 0; i < n; i++)
    {
      std::vector<Inlet *> inlets = mInputs[i];
      for (Inlet *inlet : inlets)
      {
        inlet->disconnect();
      }
    }
  }

  void Unit::connect(Unit *unit)
  {
    if (unit == NULL)
    {
      return;
    }

    int i, n;
    n = MIN(mOutputs.size(), unit->mInputs.size());
    for (i = 0; i < n; i++)
    {
      Outlet *outlet = mOutputs[i];
      if (outlet)
      {
        std::vector<Inlet *> &inlets = unit->mInputs[i];
        for (Inlet *inlet : inlets)
        {
          inlet->connect(outlet);
        }
      }
    }
  }

  void Unit::connect(Unit *u0, Unit *u1)
  {
    if (u0 == NULL || u1 == NULL)
    {
      return;
    }

    int i, n;
    n = MIN(u0->mOutputs.size(), u1->mInputs.size());
    for (i = 0; i < n; i++)
    {
      Outlet *outlet = u0->mOutputs[i];
      if (outlet)
      {
        std::vector<Inlet *> &inlets = u1->mInputs[i];
        for (Inlet *inlet : inlets)
        {
          inlet->connect(outlet);
        }
      }
    }
  }

  void Unit::process()
  {
    for (Object *o : mProcessingOrder)
    {
      o->updateParameters();
#if OBJECT_TIMING_ENABLED
      o->mExecutionTimer.start();
      o->process();
      o->mExecutionTimer.stop();
#else
      o->process();
#endif
    }
  }

  // This is the naive version (but heck a Unit has at most 20 objects).
  // When I have time implement the linear time version:
  // https://en.wikipedia.org/wiki/Topological_sorting
  bool Unit::compile()
  {
    mProcessingOrder.clear();
    mProcessingOrder.reserve(mObjects.size());

    GraphCompiler compiler;
    if (compiler.compile(mObjects, mProcessingOrder))
    {
      return true;
    }
    else
    {
      logError("Failed to compile %s.  Uncompiled objects:",
               mName.c_str());
      for (Object *o : compiler.mRemaining)
      {
        logError("%s", o->name().c_str());
      }
      return false;
    }
  }

  void Unit::addObject(Object *o)
  {
    if (o == NULL)
    {
      return;
    }

    o->attach();
#if OBJECT_TIMING_ENABLED
    Profiler::add(&o->mExecutionTimer, o->mName.c_str(), false);
#endif
    mObjects.push_back(o);
  }

  void Unit::removeObject(Object *o)
  {
    if (o == NULL)
    {
      return;
    }

    auto i = std::find(mObjects.begin(), mObjects.end(), o);
    if (i != mObjects.end())
    {
#if OBJECT_TIMING_ENABLED
      Profiler::remove(&(o->mExecutionTimer));
#endif
      mObjects.erase(i);
      o->disconnectAll();
      o->release();
    }
  }

  void Unit::hold()
  {
    for (Object *o : mObjects)
    {
      o->hold();
    }
  }

  void Unit::unhold()
  {
    for (Object *o : mObjects)
    {
      o->unhold();
    }
  }

} /* namespace od */
