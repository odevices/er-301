#pragma once
#include <od/tasks/Task.h>
#include <od/objects/Object.h>
#include <hal/concurrency/Mutex.h>

namespace od
{

  class ObjectList : public Task
  {
  public:
    ObjectList(const std::string &name);
    virtual ~ObjectList();

#ifndef SWIGLUA
    virtual void process(float *inputs, float *outputs);
#endif

    // If this task is scheduled the any modifications
    // must be bracketed by these commands.
    void lock();
    void unlock();

    // Modifying operations
    void add(Object *object);
    void remove(Object *object);
    void clear();

    // Non-modifying operations
    int size();
    void enable();
    void disable();

  private:
    std::vector<Object *> mObjects; // in processing order
    Mutex mMutex;
    bool mEnabled = true;
  };

} /* namespace od */
