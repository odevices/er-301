#pragma once

#include <od/extras/ReferenceCounted.h>
#include <od/extras/Profiler.h>
#include <od/objects/Inlet.h>
#include <od/objects/Outlet.h>
#include <od/objects/Parameter.h>
#include <od/objects/Option.h>
#include <od/objects/StateMachine.h>
#include <vector>
#include <string>

namespace od
{

  class Task;
  class Unit;
  class GraphCompiler;
  class ParamSetMorph;

  class Object : public ReferenceCounted
  {
  public:
    Object();
    virtual ~Object();

#ifndef SWIGLUA
    virtual void process();
    virtual void compile();

    void updateParameters();
    void setScheduledFlag(bool value);
#endif

    Inlet *getInput(const std::string &name);
    Outlet *getOutput(const std::string &name);
    Parameter *getParameter(const std::string &name);
    Option *getOption(const std::string &name);
    StateMachine *getStateMachine(const std::string &name);

    Inlet *getInput(int i);
    Outlet *getOutput(int i);
    Parameter *getParameter(int i);
    Option *getOption(int i);
    StateMachine *getStateMachine(int i);

    int getInputCount();
    int getOutputCount();
    int getParameterCount();
    int getOptionCount();
    int getStateMachineCount();

    bool hardSet(const std::string &name, float value);
    bool softSet(const std::string &name, float value);
    bool deserializeParameter(const std::string &name, float value);

    bool setOptionValue(const std::string &name, int choice);
    int getOptionValue(const std::string &name);

    bool hasParametersToSerialize();
    bool hasOptionsToSerialize();

    bool scheduled();

    virtual void hold();
    virtual void unhold();

    const std::string &name();
    void setName(const std::string &name);

  protected:
    std::string mName{"Object"};
    ExecutionTimer mExecutionTimer;

    std::vector<Outlet *> mOutputs;
    std::vector<Inlet *> mInputs;
    std::vector<Parameter *> mParameters;
    std::vector<Option *> mOptions;
    std::vector<StateMachine *> mStateMachines;

    void addOutput(Outlet &outlet);
    void addOutputFromHeap(Outlet *outlet);

    void addInput(Inlet &inlet);
    void addInputFromHeap(Inlet *inlet);

    void addParameter(Parameter &parameter);
    void addParameterFromHeap(Parameter *parameter);

    void addOption(Option &option);
    void addOptionFromHeap(Option *option);

    void addStateMachine(StateMachine &sm);
    void addStateMachineFromHeap(StateMachine *sm);

    void disconnectAll();

    // Compilation assistance
    int mMinDistance = 0;
    bool mMarked = false;
    bool mIsInput = false;
    bool mIsOutput = false;

    bool mIsScheduled = false;

    friend Task;
    friend Unit;
    friend GraphCompiler;
  };

  bool connect(Object &fromObject, const std::string &outletName,
               Object &toObject, const std::string &inletName);
  bool connect(Outlet &from, Inlet &to);
  void disconnect(Inlet &to);
  void disconnect(Outlet &from);

} /* namespace od */
