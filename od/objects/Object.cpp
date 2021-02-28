#include <od/objects/Object.h>

namespace od
{

  bool connect(Outlet &from, Inlet &to)
  {
    to.connect(&from);
    return true;
  }

  bool connect(Object &fromObject, const std::string &outletName,
               Object &toObject, const std::string &inletName)
  {
    Outlet *outlet = fromObject.getOutput(outletName);
    if (outlet == 0)
      return false;
    Inlet *inlet = toObject.getInput(inletName);
    if (inlet == 0)
      return false;
    inlet->connect(outlet);
    return true;
  }

  void disconnect(Inlet &to)
  {
    to.disconnect();
  }

  void disconnect(Outlet &from)
  {
    from.disconnect();
  }

  Object::Object()
  {
  }

  Object::~Object()
  {
    // disconnect all inlets
    for (Inlet *port : mInputs)
    {
      port->disconnect();
      if (port->mOnHeap)
      {
        disown(port);
      }
    }

    // disconnect all outlets
    for (Outlet *port : mOutputs)
    {
      port->disconnect();
      if (port->mOnHeap)
      {
        disown(port);
      }
    }

    // free heap parameters
    for (Parameter *param : mParameters)
    {
      param->untie();
      if (param->mOnHeap)
      {
        disown(param);
      }
    }

    // free heap options
    for (Option *option : mOptions)
    {
      option->untie();
      if (option->mOnHeap)
      {
        disown(option);
      }
    }

    // free heap states
    for (StateMachine *sm : mStateMachines)
    {
      if (sm->mOnHeap)
      {
        disown(sm);
      }
    }
  }

  void Object::disconnectAll()
  {
    // disconnect all inlets
    for (Inlet *port : mInputs)
    {
      port->disconnect();
    }

    // disconnect all outlets
    for (Outlet *port : mOutputs)
    {
      port->disconnect();
    }

    // untie parameters
    for (Parameter *param : mParameters)
    {
      param->untie();
    }

    // untie options
    for (Option *option : mOptions)
    {
      option->untie();
    }
  }

  Inlet *Object::getInput(const std::string &name)
  {
    for (Inlet *port : mInputs)
    {
      if (name == port->mName)
        return port;
    }

    return 0;
  }

  Inlet *Object::getInput(int i)
  {
    if (i < 0 || i >= (int)mInputs.size())
      return 0;
    return mInputs[i];
  }

  Outlet *Object::getOutput(const std::string &name)
  {
    for (Outlet *port : mOutputs)
    {
      if (name == port->mName)
        return port;
    }

    return 0;
  }

  Outlet *Object::getOutput(int i)
  {
    if (i < 0 || i >= (int)mOutputs.size())
      return 0;
    return mOutputs[i];
  }

  Parameter *Object::getParameter(const std::string &name)
  {
    for (Parameter *param : mParameters)
    {
      if (name == param->mName)
        return param;
    }

    return 0;
  }

  Parameter *Object::getParameter(int i)
  {
    if (i < 0 || i >= (int)mParameters.size())
      return 0;
    return mParameters[i];
  }

  Option *Object::getOption(const std::string &name)
  {
    for (Option *option : mOptions)
    {
      if (name == option->mName)
        return option;
    }

    return 0;
  }

  Option *Object::getOption(int i)
  {
    if (i < 0 || i >= (int)mOptions.size())
      return 0;
    return mOptions[i];
  }

  StateMachine *Object::getStateMachine(const std::string &name)
  {
    for (StateMachine *state : mStateMachines)
    {
      if (name == state->mName)
        return state;
    }

    return 0;
  }

  StateMachine *Object::getStateMachine(int i)
  {
    if (i < 0 || i >= (int)mStateMachines.size())
      return 0;
    return mStateMachines[i];
  }

  void Object::addOutput(Outlet &outlet)
  {
    mOutputs.push_back(&outlet);
    own(outlet); // and don't let go
  }

  void Object::addInput(Inlet &inlet)
  {
    mInputs.push_back(&inlet);
    own(inlet); // and don't let go
  }

  void Object::addOutputFromHeap(Outlet *outlet)
  {
    mOutputs.push_back(outlet);
    own(outlet);
    outlet->mOnHeap = true;
  }

  void Object::addInputFromHeap(Inlet *inlet)
  {
    mInputs.push_back(inlet);
    own(inlet);
    inlet->mOnHeap = true;
  }

  void Object::addParameter(Parameter &parameter)
  {
    mParameters.push_back(&parameter);
    own(parameter); // and don't let go
  }

  void Object::addParameterFromHeap(Parameter *parameter)
  {
    mParameters.push_back(parameter);
    own(parameter);
    parameter->mOnHeap = true;
  }

  void Object::addOption(Option &option)
  {
    mOptions.push_back(&option);
    own(option); // and don't let go
  }

  void Object::addOptionFromHeap(Option *option)
  {
    mOptions.push_back(option);
    own(option);
    option->mOnHeap = true;
  }

  void Object::addStateMachine(StateMachine &sm)
  {
    mStateMachines.push_back(&sm);
    own(sm); // and don't let go
  }

  void Object::addStateMachineFromHeap(StateMachine *sm)
  {
    mStateMachines.push_back(sm);
    own(sm);
    sm->mOnHeap = true;
  }

  bool Object::hardSet(const std::string &name, float value)
  {
    Parameter *param = getParameter(name);
    if (param)
    {
      param->hardSet(value);
      return true;
    }
    return false;
  }

  bool Object::softSet(const std::string &name, float value)
  {
    Parameter *param = getParameter(name);
    if (param)
    {
      param->softSet(value);
      return true;
    }
    return false;
  }

  bool Object::deserializeParameter(const std::string &name, float value)
  {
    Parameter *param = getParameter(name);
    if (param)
    {
      param->deserialize(value);
      return true;
    }
    return false;
  }

  bool Object::setOptionValue(const std::string &name, int choice)
  {
    Option *option = getOption(name);
    if (option)
    {
      option->set(choice);
      return true;
    }
    return false;
  }

  int Object::getOptionValue(const std::string &name)
  {
    Option *option = getOption(name);
    if (option)
    {
      return option->value();
    }
    else
    {
      return 0;
    }
  }

  bool Object::hasParametersToSerialize()
  {
    for (Parameter *param : mParameters)
    {
      if (param->isSerializationNeeded())
      {
        return true;
      }
    }
    return false;
  }

  bool Object::hasOptionsToSerialize()
  {
    for (Option *option : mOptions)
    {
      if (option->isSerializationNeeded())
      {
        return true;
      }
    }
    return false;
  }

  void Object::hold()
  {
    for (Parameter *param : mParameters)
    {
      if (param->isSerializationNeeded())
      {
        param->hold();
      }
    }
    for (Option *option : mOptions)
    {
      if (option->isSerializationNeeded())
      {
        option->hold();
      }
    }
  }

  void Object::unhold()
  {
    for (Parameter *param : mParameters)
    {
      param->unhold();
    }
    for (Option *option : mOptions)
    {
      option->unhold();
    }
  }

  bool Object::scheduled()
  {
    return mIsScheduled;
  }

  void Object::updateParameters()
  {
    for (Parameter *param : mParameters)
    {
      param->update();
    }
  }

  void Object::setScheduledFlag(bool value)
  {
    mIsScheduled = value;
  }

  void Object::process()
  {
    // Base class does nothing
  }

  void Object::compile()
  {
    // Base class does nothing
  }

  int Object::getInputCount()
  {
    return (int)mInputs.size();
  }

  int Object::getOutputCount()
  {
    return (int)mOutputs.size();
  }

  int Object::getParameterCount()
  {
    return (int)mParameters.size();
  }

  int Object::getOptionCount()
  {
    return (int)mOptions.size();
  }

  int Object::getStateMachineCount()
  {
    return (int)mStateMachines.size();
  }

  const std::string &Object::name()
  {
    return mName;
  }

  void Object::setName(const std::string &name)
  {
    mName = name;
  }

} /* namespace od */
