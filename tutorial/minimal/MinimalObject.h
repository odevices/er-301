#pragma once

#include <od/objects/Object.h>

class MinimalObject : public od::Object
{
public:
  MinimalObject();
  ~MinimalObject();

#ifndef SWIGLUA
  virtual void process();
  od::Inlet mInput{"In"};
  od::Outlet mOutput{"Out"};
#endif
};