#pragma once

#include <od/objects/Object.h>
#include <teletype/Dispatcher.h>

namespace teletype
{

  class CV : public od::Object
  {
  public:
    CV(Dispatcher *pDispatcher);
    virtual ~CV();

#ifndef SWIGLUA
    virtual void process();
    od::Outlet mOutput{"Out"};
    od::Parameter mPort{"Port", 1.0f};
#endif

  private:
    Dispatcher *mpDispatcher;
    float mLastValue = 0.0f;
  };

} // namespace teletype
