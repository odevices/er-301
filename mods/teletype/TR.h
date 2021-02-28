#pragma once

#include <od/objects/Object.h>
#include <teletype/Dispatcher.h>

namespace teletype
{

  class TR : public od::Object
  {
  public:
    TR(Dispatcher *pDispatcher);
    virtual ~TR();

#ifndef SWIGLUA
    virtual void process();
    od::Outlet mOutput{"Out"};
    od::Parameter mPort{"Port", 1.0f};
#endif

  private:
    Dispatcher *mpDispatcher;
  };

} // namespace teletype
