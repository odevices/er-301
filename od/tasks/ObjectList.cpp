#include <od/tasks/ObjectList.h>
#include <algorithm>

namespace od
{

  ObjectList::ObjectList(const std::string &name) : Task(name)
  {
  }

  ObjectList::~ObjectList()
  {
    clear();
  }

  void ObjectList::add(Object *object)
  {
    if (object == 0)
      return;
    std::vector<Object *>::iterator i = std::find(mObjects.begin(),
                                                  mObjects.end(), object);
    if (i == mObjects.end())
    {
      object->attach();
      object->setScheduledFlag(true);
      mObjects.push_back(object);
    }
  }

  void ObjectList::clear()
  {
    for (Object *object : mObjects)
    {
      object->setScheduledFlag(false);
      object->release();
    }
    mObjects.clear();
  }

  void ObjectList::remove(Object *object)
  {
    std::vector<Object *>::iterator i = std::find(mObjects.begin(),
                                                  mObjects.end(), object);
    if (i != mObjects.end())
    {
      mObjects.erase(i);
      object->setScheduledFlag(false);
      object->release();
    }
  }

  void ObjectList::process(float *inputs, float *outputs)
  {
    if (mEnabled)
    {
      mMutex.enter();
      for (Object *object : mObjects)
      {
        object->updateParameters();
        object->process();
      }
      mMutex.leave();
    }
  }

  void ObjectList::lock()
  {
    mMutex.enter();
  }

  void ObjectList::unlock()
  {
    mMutex.leave();
  }

  int ObjectList::size()
  {
    return (int)mObjects.size();
  }

  void ObjectList::enable()
  {
    mEnabled = true;
  }

  void ObjectList::disable()
  {
    mEnabled = false;
  }

} /* namespace od */
